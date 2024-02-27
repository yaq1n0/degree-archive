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

CREATE TABLE CasesAndDeathsByContinent(
  continent TEXT,
  day INT8,
  month INT8,
  year INT8,
  cases INT64,
  deaths INT64
);

CREATE TABLE CasesAndDeathsByContinentDate(
  continent TEXT,
  day INT8,
  month INT8,
  year INT8,
  cases INT64,
  deaths INT64
);

INSERT INTO CasesAndDeathsByContinent(continent, day, month, year, cases, deaths)
SELECT
  Countries.continentExp, CountryDateAlt.day, CountryDateAlt.month, CountryDateAlt.year, CountryDateAlt.cases, CountryDateAlt.deaths
FROM
  CountryDateAlt, Countries
WHERE
  CountrydateAlt.countriesAndTerritories = Countries.countriesAndTerritories;

INSERT INTO CasesAndDeathsByContinentDate(continent, day, month, year, cases, deaths)
SELECT
  continent, day, month, year, SUM(cases) AS cases, SUM(deaths) AS deaths
FROM CasesAndDeathsByContinent
GROUP BY continent, day, month, year
ORDER BY continent ASC, year ASC, month ASC, day ASC;

SELECT *
FROM CasesAndDeathsByContinentDate;

DROP TABLE CasesAndDeathsByContinent;
DROP TABLE CasesAndDeathsByContinentDate;

DROP TABLE CountryDateAlt;

COMMIT;
