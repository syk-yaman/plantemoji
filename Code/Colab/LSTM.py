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
fromDate = '2023-03-05T00:00:00.000000000Z'
toDate = '2023-03-08T23:59:59.941926044Z'

def influxDbQuery(valueName):
    client = influxdb_client.InfluxDBClient(
       url=url,
       token=token,
       org=org
    )
    query_api = client.query_api()
    query = 'from(bucket: "'+bucket+'")\
    |> range(start:' + fromDate + ' , stop:' + toDate + ' )\
    |> filter(fn: (r) => r["_field"] == "value")\
    |> filter(fn: (r) => r["host"] == "30f2640405ed")\
    |> filter(fn: (r) => r["plant-topics"] == "student/CASA0014/plant/ucfnmyr/plantemoji/'+valueName+'")'
    result = query_api.query(org=org, query=query)
    results = []
    for table in result:
      for record in table.records:
        results.append((record.get_time(), record.get_value()))
    return results
   
airHumidityQueryResult = influxDbQuery('airHumidity')
airTemperatureQueryResult = influxDbQuery('airTemperature')
soilHumidityQueryResult = influxDbQuery('soilHumidity')
soilTemperatureQueryResult = influxDbQuery('soilTemperature')
uvQueryResult = influxDbQuery('uv')

print(len(airHumidityQueryResult))
print(len(airTemperatureQueryResult))
print(len(soilHumidityQueryResult))
print(len(soilTemperatureQueryResult))
print(len(uvQueryResult))


airHumidityDataset = np.array(airHumidityQueryResult)[:, 1]
airTemperatureDataset = np.array(airTemperatureQueryResult)[:, 1]
soilHumidityDataset = np.array(soilHumidityQueryResult)[:, 1]
soilTemperatureDataset = np.array(soilTemperatureQueryResult)[:, 1]
uvDataset = np.array(uvQueryResult)[:, 1]

datetimeDataset = np.array(uvQueryResult)[:, 0]

df = pd.DataFrame({'datetime':datetimeDataset, 'airHumidity':airHumidityDataset, 'airTemperature':airTemperatureDataset, 'soilHumidity':soilHumidityDataset, 'soilTemperature':soilTemperatureDataset, 'uv':uvDataset})

#df.to_csv('importedDataset.csv')

titles = [
    "airHumidity",
    "airTemperature",
    "soilHumidity",
    "soilTemperature",
    "uv"
]

feature_keys = [
    "airHumidity",
    "airTemperature",
    "soilHumidity",
    "soilTemperature",
    "uv"
]

colors = [
    "blue",
    "orange",
    "green",
    "red",
    "purple"
]

date_time_key = "datetime"

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

df2 = pd.DataFrame({'airHumidity':airHumidityDataset, 'airTemperature':airTemperatureDataset, 'soilHumidity':soilHumidityDataset, 'soilTemperature':soilTemperatureDataset, 'uv':uvDataset})
#Be careful before calculating correlation to chech the dtypes of the dataframe, even if you see them as floats in df.describe(),
#that doesn't mean they are floats!
df.dtypes
df2 = df2.apply(pd.to_numeric)

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
