BEGIN TRANSACTION;

CREATE TABLE CountryCaseDeaths(
  country TEXT,
  date TEXT,
  cases INT64,
  deaths INT64
);

CREATE TABLE CountryPopulations(
  country TEXT,
  population INT16,
  cases INT64,
  deaths INT64
);

CREATE TABLE Percentages(
  country TEXT,
  case_per REAL,
  deaths_per REAL
);

INSERT INTO CountryCaseDeaths(country, date, cases, deaths)
SELECT
  countriesAndTerritories, dateRep, SUM(cases) as cases, SUM(deaths) as deaths
FROM
  CountryDate
GROUP BY countriesAndTerritories;

INSERT INTO CountryPopulations(country, population, cases, deaths)
SELECT
  CountryCaseDeaths.country, Countries.popData2019, CountryCaseDeaths.cases, CountryCaseDeaths.deaths
FROM
  CountryCaseDeaths, Countries
Where
  CountryCaseDeaths.country = Countries.countriesAndTerritories;

INSERT INTO Percentages(country, case_per, deaths_per)
SELECT
  country, (((cases * 1.0) / (population * 1.0)) * 100.0), (((deaths * 1.0) / (population * 1.0)) * 100.0)
FROM
  CountryPopulations
GROUP BY country;

SELECT *
FROM Percentages;

DROP TABLE CountryCaseDeaths;
DROP TABLE CountryPopulations;
DROP TABLE Percentages;

COMMIT;
