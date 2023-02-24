//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/rewriter_extension.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/main/connection.hpp"

namespace duckdb
{

//! The RewriterExtensionInfo holds static information relevant to the optimizer extension
struct RewriterExtensionInfo {
	DUCKDB_API virtual ~RewriterExtensionInfo() {
	}
};

typedef string (*rewriter_function_t)(Connection &connection, RewriterExtensionInfo *info, const string &query);
typedef void (*set_connections_function_t)(RewriterExtensionInfo *info, string &input);

class RewriterExtension {
public:
	//! The rewriter function of the rewriter extension.
	rewriter_function_t rewriter_function;

	//! The set connection of the rewriter extension.
	set_connections_function_t set_connections;

	//! Additional rewriter info passed to the parse function
	shared_ptr<RewriterExtensionInfo> rewriter_info;

};
    
} // namespace duckdb
