#ifndef GROUPSCHOLAR_POLICY_CHANGE_TRACKER_H
#define GROUPSCHOLAR_POLICY_CHANGE_TRACKER_H

#include <string>
#include <vector>

struct PolicyChangeInput {
  std::string title;
  std::string category;
  std::string impact_level;
  std::string effective_date;
  std::string owner;
  std::string notes;
};

struct ReportRow {
  std::string category;
  int count;
};

struct QueryFilters {
  std::string category;
  std::string impact_level;
  std::string owner;
  std::string since_date;
  std::string until_date;
};

std::string sanitize_for_sql(const std::string &value);
std::string require_non_empty(const std::string &label, const std::string &value);
std::string build_insert_sql(const PolicyChangeInput &input);
std::string build_schema_sql();
std::string build_seed_sql();
std::string build_where_clause(const QueryFilters &filters,
                               const std::vector<std::string> &base_conditions);
std::string resolve_report_group_column(const std::string &by);
std::vector<ReportRow> parse_report_rows(const std::string &raw);

#endif
