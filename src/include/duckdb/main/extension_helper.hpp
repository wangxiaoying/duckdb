//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/main/extension_helper.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include "duckdb.hpp"

namespace duckdb {
class DuckDB;

enum class ExtensionLoadResult : uint8_t { LOADED_EXTENSION = 0, EXTENSION_UNKNOWN = 1, NOT_LOADED = 2 };

struct DefaultExtension {
	const char *name;
	const char *description;
	bool statically_loaded;
};

struct ExtensionAlias {
	const char *alias;
	const char *extension;
};

struct ExtensionInitResult {
	string filename;
	string basename;

	void *lib_hdl;
};

class ExtensionHelper {
public:
	static void LoadAllExtensions(DuckDB &db);

	static ExtensionLoadResult LoadExtension(DuckDB &db, const std::string &extension);

	static void InstallExtension(ClientContext &context, const string &extension, bool force_install);
	static void InstallExtension(DBConfig &config, FileSystem &fs, const string &extension, bool force_install);
	static void LoadExternalExtension(ClientContext &context, const string &extension);
	static void LoadExternalExtension(DatabaseInstance &db, FileOpener *opener, const string &extension);

	static string ExtensionDirectory(ClientContext &context);
	static string ExtensionDirectory(DBConfig &config, FileSystem &fs, FileOpener *opener);

	static idx_t DefaultExtensionCount();
	static DefaultExtension GetDefaultExtension(idx_t index);

	static idx_t ExtensionAliasCount();
	static ExtensionAlias GetExtensionAlias(idx_t index);

	static const vector<string> GetPublicKeys();

	// Returns extension name, or empty string if not a replacement open path
	static string ExtractExtensionPrefixFromPath(const string &path);

	//! Apply any known extension aliases
	static string ApplyExtensionAlias(string extension_name);

	static string GetExtensionName(const string &extension);
	static bool IsFullPath(const string &extension);

private:
	static void InstallExtensionInternal(DBConfig &config, ClientConfig *client_config, FileSystem &fs,
	                                     const string &local_path, const string &extension, bool force_install);
	static const vector<string> PathComponents();
	static bool AllowAutoInstall(const string &extension);
	static ExtensionInitResult InitialLoad(DBConfig &config, FileOpener *opener, const string &extension);
	static bool TryInitialLoad(DBConfig &config, FileOpener *opener, const string &extension,
	                           ExtensionInitResult &result, string &error);
	//! For tagged releases we use the tag, else we use the git commit hash
	static const string GetVersionDirectoryName();
	//! Version tags occur with and without 'v', tag in extension path is always with 'v'
	static const string NormalizeVersionTag(const string &version_tag);
	static bool IsRelease(const string &version_tag);
	static bool CreateSuggestions(const string &extension_name, string &message);

private:
	static ExtensionLoadResult LoadExtensionInternal(DuckDB &db, const std::string &extension, bool initial_load);
};

} // namespace duckdb
