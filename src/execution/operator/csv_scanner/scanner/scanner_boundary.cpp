#include "duckdb/execution/operator/scan/csv/scanner/scanner_boundary.hpp"

namespace duckdb {

CSVPosition::CSVPosition(idx_t file_idx_p, idx_t buffer_idx_p, idx_t buffer_pos_p)
    : file_idx(file_idx_p), buffer_idx(buffer_idx_p), buffer_pos(buffer_pos_p) {
}
CSVPosition::CSVPosition() {
}

CSVBoundary::CSVBoundary(idx_t file_idx_p, idx_t buffer_idx_p, idx_t buffer_pos_p, idx_t boundary_idx_p,
                         idx_t end_pos_p)
    : file_idx(file_idx_p), buffer_idx(buffer_idx_p), buffer_pos(buffer_pos_p), boundary_idx(boundary_idx_p),
      end_pos(end_pos_p) {
}
CSVBoundary::CSVBoundary() : file_idx(0), buffer_idx(0), buffer_pos(0), boundary_idx(0), end_pos(0) {
}
CSVIterator::CSVIterator(idx_t file_idx, idx_t buffer_idx, idx_t buffer_pos, idx_t boundary_idx)
    : pos(file_idx, buffer_idx, buffer_pos),
      boundary(file_idx, buffer_idx, buffer_pos, boundary_idx, buffer_pos + BYTES_PER_THREAD), is_set(true) {};

CSVIterator::CSVIterator() : is_set(false) {};

void CSVBoundary::Print() {
	std::cout << "---Boundary: " << boundary_idx << " ---" << std::endl;
	std::cout << "File Index:: " << file_idx << std::endl;
	std::cout << "Buffer Index: " << buffer_idx << std::endl;
	std::cout << "Buffer Pos: " << buffer_pos << std::endl;
	std::cout << "End Pos: " << end_pos << std::endl;
	std::cout << "------------" << end_pos << std::endl;
}

void CSVIterator::Print() {
	boundary.Print();
	std::cout << "Is set: " << is_set << std::endl;
}

bool CSVIterator::Next(CSVBufferManager &buffer_manager) {
	if (boundary.file_idx >= buffer_manager.FileCount()) {
		// We are done
		return true;
	}
	boundary.boundary_idx++;
	// This is our start buffer
	auto buffer = buffer_manager.GetBuffer(boundary.file_idx, boundary.buffer_idx);
	if (buffer->is_last_buffer && boundary.buffer_pos + CSVIterator::BYTES_PER_THREAD > buffer->actual_size) {
		// 1) We are done with the current file, we must move to the next file
		// We are done with this file, we need to reset everything for the next file
		boundary.file_idx++;
		if (boundary.file_idx >= buffer_manager.FileCount()) {
			// We are done
			return true;
		}
		boundary.buffer_idx = 0;
		boundary.buffer_pos = buffer_manager.GetStartPos();
	} else if (boundary.buffer_pos + BYTES_PER_THREAD >= buffer->actual_size) {
		// 2) We still have data to scan in this file, we set the iterator accordingly.
		// We must move the buffer
		boundary.buffer_idx++;
		boundary.buffer_pos = buffer_manager.GetStartPos();

	} else {
		// 3) We are not done with the current buffer, hence we just move where we start within the buffer
		boundary.buffer_pos += BYTES_PER_THREAD;
	}
	boundary.end_pos = boundary.buffer_pos + BYTES_PER_THREAD;
	SetCurrentPositionToBoundary();
	return false;
}

bool CSVIterator::IsSet() const {
	return is_set;
}
idx_t CSVIterator::GetEndPos() const {
	return boundary.end_pos;
}

idx_t CSVIterator::GetFileIdx() const {
	return pos.file_idx;
}

idx_t CSVIterator::GetBufferIdx() const {
	return boundary.buffer_idx;
}

idx_t CSVIterator::GetBoundaryIdx() const {
	return boundary.boundary_idx;
}

void CSVIterator::SetCurrentPositionToBoundary() {
	pos.file_idx = boundary.file_idx;
	pos.buffer_idx = boundary.buffer_idx;
	pos.buffer_pos = boundary.buffer_pos;
}

} // namespace duckdb
