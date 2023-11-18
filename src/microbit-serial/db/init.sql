CREATE DATABASE IF NOT EXISTS microbit;
USE microbit;

CREATE TABLE IF NOT EXISTS sensor_data (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp TIMESTAMP,
    temperature FLOAT,
    lux FLOAT
);