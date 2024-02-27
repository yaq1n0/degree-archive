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

CREATE TABLE UKCaseDeath(
  country TEXT,
  day INT8,
  month INT8,
  year INT8,
  cases INT64,
  deaths INT64
);

CREATE TABLE UKCaseDeathTotal(
  day INT8,
  month INT8,
  year INT8,
  cases INT64,
  deaths INT64
);

INSERT INTO UKCaseDeath(country, day, month, year, cases, deaths)
SELECT
  countriesAndTerritories, day, month, year, cases, deaths
FROM
  CountryDateAlt
WHERE
  countriesAndTerritories='United_Kingdom'
ORDER BY
  year ASC, month ASC, day ASC;

INSERT INTO UKCaseDeathTotal(day, month, year, cases, deaths)
SELECT
  day, month, year,
  SUM(UKCaseDeath.cases) OVER (ORDER BY year ASC, month ASC, day ASC) AS cases,
  SUM(UKCaseDeath.deaths) OVER (ORDER BY year ASC, month ASC, day ASC) AS deaths
FROM UKCaseDeath;

SELECT * FROM UKCaseDeathTotal;

DROP TABLE UKCaseDeath;
DROP TABLE UKCaseDeathTotal;

DROP TABLE CountryDateAlt;

COMMIT
