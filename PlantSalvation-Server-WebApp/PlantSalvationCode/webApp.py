from flask import Flask, render_template
import psycopg2
import json

app = Flask(__name__)


# Database connection helper
def get_db_connection():
    return psycopg2.connect(
        database="plantsalvation",
        user="postgres",
        password="YOUR PASSWORD ",
        host="localhost",
        port="5432"
    )

# Homepage route
@app.route("/")
def home():
    return render_template("index.html")



# Update page 
@app.route("/api/data")
def api_data():
    #connect to db
    conn = get_db_connection()
    cur = conn.cursor()

    # Limit 200 to get 50 datapoints for each sensor
    # Remove limit to see all data
    cur.execute("""
        SELECT s.type, sr.reading_value, sr.reading_time
        FROM sensor_reading sr
        JOIN sensor s ON s.sensor_id = sr.sensor_id
        ORDER BY sr.reading_time DESC
        limit 200;
    """)

    rows = cur.fetchall()
    
    #Reverse Data to have new data points appear on the right side of the graph
    rows.reverse()
    cur.close()
    conn.close()

    sensor_data = {}

    for s_type, value, timestamp in rows:
        if s_type not in sensor_data:
            sensor_data[s_type] = {
                "labels": [],
                "values": []
            }

        sensor_data[s_type]["labels"].append(
            timestamp.strftime("%Y-%m-%d %H:%M:%S")
        )
        sensor_data[s_type]["values"].append(float(value))

    return sensor_data


# Data dashboard route
@app.route("/data")
def data():
    conn = get_db_connection()
    cur = conn.cursor()

    cur.execute("""
        SELECT s.name, s.type, sr.reading_value, sr.reading_time
        FROM sensor_reading sr
        JOIN sensor s ON s.sensor_id = sr.sensor_id
        ORDER BY sr.reading_time DESC
        LIMIT 200;
    """)

    rows = cur.fetchall()
    rows.reverse()
    cur.close()
    conn.close()

    # Group data by sensor type
    sensor_data = {}

    for name, s_type, value, timestamp in rows:
        if s_type not in sensor_data:
            sensor_data[s_type] = {
                "labels": [],
                "values": []
            }

        sensor_data[s_type]["labels"].append(
            timestamp.strftime("%Y-%m-%d %H:%M")
        )
        sensor_data[s_type]["values"].append(float(value))
        

    return render_template(
        "plantData.html",
        sensor_data=json.dumps(sensor_data)
    )


if __name__ == "__main__":
    app.run(debug=True)
