#include "policy_tracker.h"

#include <sstream>
#include <stdexcept>

std::string sanitize_for_sql(const std::string &value) {
  std::string escaped;
  escaped.reserve(value.size());
  for (char c : value) {
    if (c == '\'' ) {
      escaped.push_back('\'');
    }
    escaped.push_back(c);
  }
  return escaped;
}

std::string require_non_empty(const std::string &label, const std::string &value) {
  if (value.empty()) {
    throw std::runtime_error(label + " is required");
  }
  return value;
}

std::string build_insert_sql(const PolicyChangeInput &input) {
  std::ostringstream sql;
  sql << "insert into groupscholar_policy_change_tracker.policy_changes "
         "(title, category, impact_level, effective_date, owner, notes) values (";
  sql << "'" << sanitize_for_sql(require_non_empty("title", input.title)) << "', ";
  sql << "'" << sanitize_for_sql(require_non_empty("category", input.category)) << "', ";
  sql << "'" << sanitize_for_sql(require_non_empty("impact_level", input.impact_level)) << "', ";
  sql << "'" << sanitize_for_sql(require_non_empty("effective_date", input.effective_date)) << "', ";
  sql << "'" << sanitize_for_sql(require_non_empty("owner", input.owner)) << "', ";
  sql << "'" << sanitize_for_sql(input.notes) << "');";
  return sql.str();
}

std::string build_schema_sql() {
  return "create schema if not exists groupscholar_policy_change_tracker;\n"
         "create table if not exists groupscholar_policy_change_tracker.policy_changes ("
         "id bigserial primary key,"
         "title text not null,"
         "category text not null,"
         "impact_level text not null,"
         "effective_date date not null,"
         "owner text not null,"
         "notes text not null default '',"
         "created_at timestamptz not null default now()"
         ");";
}

std::string build_seed_sql() {
  return "insert into groupscholar_policy_change_tracker.policy_changes "
         "(title, category, impact_level, effective_date, owner, notes) values "
         "('FAFSA dependency rule update','Compliance','High','2026-01-15','Policy Ops',"
         "'Requires re-validation for dependent status.'),"
         "('Essay rubric weighting tweak','Review Ops','Medium','2026-01-20','Review Lead',"
         "'Boost weight for community impact from 15% to 20%.'),"
         "('Renewal documentation simplification','Renewals','Low','2026-02-01','Program Manager',"
         "'Removed redundant income verification for continuing scholars.');";
}

std::vector<ReportRow> parse_report_rows(const std::string &raw) {
  std::vector<ReportRow> rows;
  std::istringstream stream(raw);
  std::string line;
  while (std::getline(stream, line)) {
    if (line.empty()) {
      continue;
    }
    auto sep = line.find('|');
    if (sep == std::string::npos) {
      continue;
    }
    ReportRow row;
    row.category = line.substr(0, sep);
    row.count = std::stoi(line.substr(sep + 1));
    rows.push_back(row);
  }
  return rows;
}
