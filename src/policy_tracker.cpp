#include "policy_tracker.h"

#include <sstream>
#include <stdexcept>
#include <vector>

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

std::string build_select_by_id_sql(long id) {
  if (id <= 0) {
    throw std::runtime_error("id must be a positive integer");
  }
  std::ostringstream sql;
  sql << "select id, title, category, impact_level, effective_date, owner, notes, created_at "
         "from groupscholar_policy_change_tracker.policy_changes where id = "
      << id << ";";
  return sql.str();
}

std::string build_where_clause(const QueryFilters &filters,
                               const std::vector<std::string> &base_conditions) {
  std::vector<std::string> conditions = base_conditions;
  if (!filters.category.empty()) {
    conditions.push_back("category = '" + sanitize_for_sql(filters.category) + "'");
  }
  if (!filters.impact_level.empty()) {
    conditions.push_back("impact_level = '" + sanitize_for_sql(filters.impact_level) + "'");
  }
  if (!filters.owner.empty()) {
    conditions.push_back("owner = '" + sanitize_for_sql(filters.owner) + "'");
  }
  if (!filters.since_date.empty()) {
    conditions.push_back("effective_date >= '" + sanitize_for_sql(filters.since_date) + "'");
  }
  if (!filters.until_date.empty()) {
    conditions.push_back("effective_date <= '" + sanitize_for_sql(filters.until_date) + "'");
  }
  if (conditions.empty()) {
    return "";
  }
  std::ostringstream sql;
  sql << " where ";
  for (size_t i = 0; i < conditions.size(); ++i) {
    if (i > 0) {
      sql << " and ";
    }
    sql << conditions[i];
  }
  return sql.str();
}

std::string resolve_report_group_column(const std::string &by) {
  if (by == "impact") {
    return "impact_level";
  }
  if (by == "owner") {
    return "owner";
  }
  return "category";
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
