#include "policy_tracker.h"

#include <libpq-fe.h>

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void print_usage() {
  std::cout
      << "groupscholar-policy-change-tracker\n\n"
      << "Commands:\n"
      << "  init                      Create schema and tables\n"
      << "  seed                      Insert sample policy changes\n"
      << "  add --title <title> --category <category> --impact <level> \\n--effective-date <YYYY-MM-DD> --owner <owner> [--notes <notes>]\n"
      << "  list [--limit <n>]         List recent policy changes\n"
      << "  report                    Summarize changes by category\n";
}

void ensure_ok(PGconn *conn, PGresult *result) {
  (void)conn;
  if (!result) {
    throw std::runtime_error("query failed: empty result");
  }
  auto status = PQresultStatus(result);
  if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
    std::string message = PQresultErrorMessage(result);
    PQclear(result);
    throw std::runtime_error("query failed: " + message);
  }
}

PGresult *exec_or_throw(PGconn *conn, const std::string &sql) {
  PGresult *result = PQexec(conn, sql.c_str());
  ensure_ok(conn, result);
  return result;
}

std::map<std::string, std::string> parse_flags(int argc, char **argv) {
  std::map<std::string, std::string> flags;
  for (int i = 2; i < argc; ++i) {
    std::string key(argv[i]);
    if (key.rfind("--", 0) == 0) {
      if (i + 1 >= argc) {
        throw std::runtime_error("Missing value for " + key);
      }
      flags[key] = argv[i + 1];
      ++i;
    }
  }
  return flags;
}

PolicyChangeInput input_from_flags(const std::map<std::string, std::string> &flags) {
  PolicyChangeInput input;
  auto fetch = [&](const std::string &key) -> std::string {
    auto it = flags.find(key);
    if (it == flags.end()) {
      return "";
    }
    return it->second;
  };
  input.title = fetch("--title");
  input.category = fetch("--category");
  input.impact_level = fetch("--impact");
  input.effective_date = fetch("--effective-date");
  input.owner = fetch("--owner");
  input.notes = fetch("--notes");
  return input;
}

void print_list(PGresult *result) {
  int rows = PQntuples(result);
  std::cout << "ID | Effective | Category | Impact | Title | Owner\n";
  for (int i = 0; i < rows; ++i) {
    std::cout << PQgetvalue(result, i, 0) << " | "
              << PQgetvalue(result, i, 1) << " | "
              << PQgetvalue(result, i, 2) << " | "
              << PQgetvalue(result, i, 3) << " | "
              << PQgetvalue(result, i, 4) << " | "
              << PQgetvalue(result, i, 5) << "\n";
  }
}

void print_report(PGresult *result) {
  int rows = PQntuples(result);
  std::cout << "Category | Count\n";
  for (int i = 0; i < rows; ++i) {
    std::cout << PQgetvalue(result, i, 0) << " | " << PQgetvalue(result, i, 1)
              << "\n";
  }
}
} // namespace

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage();
    return 1;
  }

  std::string command(argv[1]);
  if (command == "--help" || command == "-h") {
    print_usage();
    return 0;
  }

  PGconn *conn = PQconnectdb("");
  if (PQstatus(conn) != CONNECTION_OK) {
    std::cerr << "Database connection failed: " << PQerrorMessage(conn) << "\n";
    PQfinish(conn);
    return 1;
  }

  try {
    if (command == "init") {
      auto sql = build_schema_sql();
      auto result = exec_or_throw(conn, sql);
      PQclear(result);
      std::cout << "Schema ready.\n";
    } else if (command == "seed") {
      auto sql = build_seed_sql();
      auto result = exec_or_throw(conn, sql);
      PQclear(result);
      std::cout << "Seed data inserted.\n";
    } else if (command == "add") {
      auto flags = parse_flags(argc, argv);
      auto input = input_from_flags(flags);
      auto sql = build_insert_sql(input);
      auto result = exec_or_throw(conn, sql);
      PQclear(result);
      std::cout << "Policy change added.\n";
    } else if (command == "list") {
      int limit = 20;
      auto flags = parse_flags(argc, argv);
      auto it = flags.find("--limit");
      if (it != flags.end()) {
        limit = std::stoi(it->second);
      }
      std::string sql =
          "select id, effective_date, category, impact_level, title, owner "
          "from groupscholar_policy_change_tracker.policy_changes "
          "order by effective_date desc, id desc limit " +
          std::to_string(limit) + ";";
      auto result = exec_or_throw(conn, sql);
      print_list(result);
      PQclear(result);
    } else if (command == "report") {
      std::string sql =
          "select category, count(*) from "
          "groupscholar_policy_change_tracker.policy_changes "
          "group by category order by count desc;";
      auto result = exec_or_throw(conn, sql);
      print_report(result);
      PQclear(result);
    } else {
      std::cerr << "Unknown command: " << command << "\n";
      print_usage();
      PQfinish(conn);
      return 1;
    }
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    PQfinish(conn);
    return 1;
  }

  PQfinish(conn);
  return 0;
}
