CREATE DATABASE plantsalvation;

CREATE TABLE IF NOT EXISTS sensor (
    sensor_id INTEGER PRIMARY KEY,
    name VARCHAR(55) NOT NULL,
    type VARCHAR(55) NOT NULL
);

CREATE TABLE IF NOT EXISTS sensor_reading (
    reading_id SERIAL PRIMARY KEY,          
    sensor_id INTEGER NOT NULL,           
    reading_time TIMESTAMP NOT NULL,       
    reading_value DOUBLE PRECISION NOT NULL,

    FOREIGN KEY (sensor_id) REFERENCES sensor(sensor_id)
);

INSERT INTO sensor (sensor_id, name, type)
VALUES
    (1, 'Temperature Sensor', 'temp'),
    (2, 'Humidity Sensor', 'humidity'),
    (3, 'Soil Moisture Sensor', 'moisturePercent'),
    (4, 'Light Sensor', 'lightPercent');