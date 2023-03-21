import pandas as pd
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow import keras
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS

from zipfile import ZipFile
import os

bucket = "-----"
org = "-----"
token = "-----"
# Store the URL of your InfluxDB instance
url="----"

def influxDbQuery(valueName):
    client = influxdb_client.InfluxDBClient(
       url=url,
       token=token,
       org=org
    )
    query_api = client.query_api()
    query = 'from(bucket: "'+bucket+'")\
    |> range(start: 2023-02-24T00:00:00.000000000Z, stop: 2023-02-25T23:59:59.941926044Z)\
    |> filter(fn: (r) => r["_field"] == "value")\
    |> filter(fn: (r) => r["host"] == "30f2640405ed")\
    |> filter(fn: (r) => r["plant-topics"] == "student/CASA0014/plant/ucfnmyr/plantemoji/'+valueName+'")'
    result = query_api.query(org=org, query=query)
    results = []
    for table in result:
      for record in table.records:
        results.append((record.get_time(), record.get_value()))
    return results
   
airHumidityResult = influxDbQuery('airHumidity')
airTemperatureResult = influxDbQuery('airTemperature')
soilHumidityResult = influxDbQuery('soilHumidity')
soilTemperatureResult = influxDbQuery('soilTemperature')
uvResult = influxDbQuery('uv')

print(len(airHumidityResult))
print(len(airTemperatureResult))
print(len(soilHumidityResult))
print(len(soilTemperatureResult))
print(len(uvResult))


airHumidityDataset = np.array(airHumidityResult)[:, 1]
airTemperatureDataset = np.array(airTemperatureResult)[:, 1]
soilHumidityDataset = np.array(soilHumidityResult)[:, 1]
soilTemperatureDataset = np.array(soilTemperatureResult)[:, 1]
uvDataset = np.array(uvResult)[:, 1]


df = pd.DataFrame({'airHumidity':airHumidityDataset, 'airTemperature':airTemperatureDataset, 'soilHumidity':soilHumidityDataset, 'soilTemperature':soilTemperatureDataset, 'uv':uvDataset})

array = np.array(results)


df = pd.DataFrame(array[:, 1], columns=['humidity'])
print(df)
df.plot()

df = 

titles = [
    "Pressure",
    "Temperature",
    "Temperature in Kelvin",
    "Temperature (dew point)",
    "Relative Humidity",
    "Saturation vapor pressure",
    "Vapor pressure",
    "Vapor pressure deficit",
    "Specific humidity",
    "Water vapor concentration",
    "Airtight",
    "Wind speed",
    "Maximum wind speed",
    "Wind direction in degrees",
]

feature_keys = [
    "p (mbar)",
    "T (degC)",
    "Tpot (K)",
    "Tdew (degC)",
    "rh (%)",
    "VPmax (mbar)",
    "VPact (mbar)",
    "VPdef (mbar)",
    "sh (g/kg)",
    "H2OC (mmol/mol)",
    "rho (g/m**3)",
    "wv (m/s)",
    "max. wv (m/s)",
    "wd (deg)",
]

colors = [
    "blue",
    "orange",
    "green",
    "red",
    "purple",
    "brown",
    "pink",
    "gray",
    "olive",
    "cyan",
]

date_time_key = "Date Time"

def show_raw_visualization(data):
    time_data = data[date_time_key]
    fig, axes = plt.subplots(
        nrows=7, ncols=2, figsize=(15, 20), dpi=80, facecolor="w", edgecolor="k"
    )
    for i in range(len(feature_keys)):
        key = feature_keys[i]
        c = colors[i % (len(colors))]
        t_data = data[key]
        t_data.index = time_data
        t_data.head()
        ax = t_data.plot(
            ax=axes[i // 2, i % 2],
            color=c,
            title="{} - {}".format(titles[i], key),
            rot=25,
        )
        ax.legend([titles[i]])
    plt.tight_layout()


show_raw_visualization(df)


def show_heatmap(data):
    plt.matshow(data.corr())
    plt.xticks(range(data.shape[1]), data.columns, fontsize=14, rotation=90)
    plt.gca().xaxis.tick_bottom()
    plt.yticks(range(data.shape[1]), data.columns, fontsize=14)

    cb = plt.colorbar()
    cb.ax.tick_params(labelsize=14)
    plt.title("Feature Correlation Heatmap", fontsize=14)
    plt.show()

show_heatmap(df)
