BEGIN TRANSACTION;

CREATE TABLE CountryDateAlt(
  day INT8,
  month INT8,
  year INT8,
  countriesAndTerritories TEXT,
  cases INT64,
  deaths INT64
);

INSERT OR IGNORE INTO CountryDateAlt(day, month, year, countriesAndTerritories, cases, deaths)
SELECT
  day, month, year, countriesAndTerritories, cases, deaths
FROM
  dataset;

CREATE TABLE UKCasesByIncreasingDate(
  day INT8,
  month INT8,
  year INT8,
  cases INT64
);

INSERT INTO UKCasesByIncreasingDate(day, month, year, cases)
SELECT
  day, month, year, cases
FROM
  CountryDateAlt
WHERE
  countriesAndTerritories='United_Kingdom'
ORDER BY
  year ASC, month ASC, day ASC;

SELECT *
FROM UKCasesByIncreasingDate;

DROP TABLE UKCasesByIncreasingDate;

DROP TABLE CountryDateAlt;

COMMIT;
