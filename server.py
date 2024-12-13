import os
import csv
from flask import Flask, request, jsonify
from datetime import datetime
app = Flask(__name__)
desktop_path = os.path.join(os.path.expanduser("~"), "Desktop")
folder_name = "TemperatureData"
folder_path = os.path.join(desktop_path, folder_name)
csv_file_path = os.path.join(folder_path, "temperature_data.csv")
if not os.path.exists(folder_path):
    os.makedirs(folder_path)
if not os.path.exists(csv_file_path):
    with open(csv_file_path, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Timestamp", "Temperature"])
@app.route('/', methods=['GET'])
def record_temperature():
    temp = request.args.get('temp')
    if temp is None:
        return jsonify({"error": "Missing 'temp' parameter"}), 400

    try:
        temperature = float(temp)
    except ValueError:
        return jsonify({"error": "Invalid 'temp' parameter. Must be a number."} >

                       timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        with open(csv_file_path, mode='a', newline='') as file:
            writer = csv.writer(file)
        writer.writerow([timestamp, temperature])

    return jsonify({"message": "Data recorded successfully", "timestamp": times >
