import mysql.connector
import json

class DatabaseManager:
    def __init__(self, config_file='db_config.json'):
        self.config = self.load_config(config_file)
        self.connection = None
        self.cursor = None

    def load_config(self, config_file):
        with open(config_file, 'r') as file:
            return json.load(file)['database']

    def connect(self):
        try:
            self.connection = mysql.connector.connect(
                host=self.config['host'],
                user=self.config['user'],
                password=self.config['password']
            )
            self.cursor = self.connection.cursor()
            print("DB MANAGER - Connected to:", self.connection.get_server_info())
            self.cursor.execute("CREATE DATABASE IF NOT EXISTS {}".format(self.config['database_name']))
            self.cursor.execute("USE {}".format(self.config['database_name']))
            self.cursor.execute("CREATE TABLE IF NOT EXISTS sensor_data (id INT AUTO_INCREMENT PRIMARY KEY, timestamp TIMESTAMP, temperature FLOAT, lux FLOAT);")
        except mysql.connector.Error as err:
            print("Error while connecting to database :", err)
            exit(1)

    def insert_data(self, temperature, lux, timestamp):
        query = "INSERT INTO sensor_data (temperature, lux, timestamp) VALUES (%s, %s, %s)"
        values = (temperature, lux, timestamp)
        try:
            self.cursor.execute(query, values)
            self.connection.commit()
            return True
        except mysql.connector.Error as err:
            print("Error while inserting data in database :", err)
            self.connection.rollback()
            return False

    def get_last_value(self):
        self.cursor.execute("SELECT * FROM sensor_data ORDER BY id DESC LIMIT 1")
        last_value = self.cursor.fetchone()
        last_timestamp = last_value[1]
        last_temp = last_value[2]
        last_lux = last_value[3]
        return last_temp, last_lux, last_timestamp

    def print_all_values(self):
        self.cursor.execute("SELECT * FROM sensor_data")
        allvalues = self.cursor.fetchall()
        for value in allvalues:
            print(value)

    def print_last_ten_values(self):
        self.cursor.execute("SELECT * FROM sensor_data ORDER BY id DESC LIMIT 10")
        allvalues = self.cursor.fetchall()
        for value in allvalues:
            print(value)

    def close(self):
        self.cursor.close()
        self.connection.close()
