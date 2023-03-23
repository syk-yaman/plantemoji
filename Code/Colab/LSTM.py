#This code is based on https://github.com/google-coral/tutorials/blob/master/train_lstm_timeseries_ptq_tf2.ipynb
#and https://github.com/djdunc/casa0018/blob/main/Week7/CASA0018_7_lab_solution.ipynb

import pandas as pd
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow import keras
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
import numpy as np
from sklearn.preprocessing import MinMaxScaler
from zipfile import ZipFile
import os

############################ SECTION: Importing dataset from InfluxDB ############################
bucket = "INSERT_HERE"
org = "INSERT_HERE"
token = "INSERT_HERE"
# Store the URL of your InfluxDB instance
url="INSERT_HERE"
fromDate = '2023-02-22T00:00:00.000000000Z'
toDate = '2023-03-20T23:59:59.941926044Z'

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

#if not all results are equal in length:
#airHumidityQueryResult = airHumidityQueryResult[:-14]

print(len(airHumidityQueryResult))
print(len(airTemperatureQueryResult))
print(len(soilHumidityQueryResult))
print(len(soilTemperatureQueryResult))
print(len(uvQueryResult))

############################ SECTION: Converting the shape of the dataset ############################

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

#remove datetime column 
df2 = pd.DataFrame({'airHumidity':airHumidityDataset, 'airTemperature':airTemperatureDataset, 'soilHumidity':soilHumidityDataset, 'soilTemperature':soilTemperatureDataset, 'uv':uvDataset})

#NOTE: Be careful before calculating correlation to chech the dtypes of the dataframe, even if you see them as floats in df.describe(),
#that doesn't mean they are floats!
df2.dtypes
df2 = df2.apply(pd.to_numeric)
df2.dtypes

############################ SECTION: Trying to understand dataset's correlation ############################

def show_heatmap(data, method):
    plt.matshow(data.corr(method))
    plt.xticks(range(data.shape[1]), data.columns, fontsize=14, rotation=90)
    plt.gca().xaxis.tick_bottom()
    plt.yticks(range(data.shape[1]), data.columns, fontsize=14)
    cb = plt.colorbar()
    cb.ax.tick_params(labelsize=14)
    plt.title("Feature Correlation Heatmap", fontsize=14)
    plt.show()

show_heatmap(df2, 'pearson')


############################ SECTION: Splitting the dataset and normalise it ############################

split_fraction = 0.715
train_split = int(split_fraction * int(df.shape[0]))
step = 50

past = 7200
future = 720
learning_rate = 0.001
batch_size = 256

def normalize(data, train_split):
    data_mean = data[:train_split].mean(axis=0)
    data_std = data[:train_split].std(axis=0)
    return (data - data_mean) / data_std

print(
    "The selected parameters are:",
    ", ".join([titles[i] for i in [0, 1, 2, 3, 4]]),
)
selected_features = [feature_keys[i] for i in [0, 1, 2, 3, 4]]
features = df2[selected_features] #using the corrected dtypes
#features.index = df[date_time_key]
features.head()





#features = normalize(features.values, train_split)
features = pd.DataFrame(features)
#features.columns = features.loc[0]
#features = features.drop(0)
features.head()

scaler = MinMaxScaler(copy=True, feature_range=(0, 1))
scaler.fit(features.values)


train_data = features.loc[0 : train_split - 1]
val_data = features.loc[train_split:]

train_data = scaler.transform(train_data)
val_data = scaler.transform(val_data)

train_data = pd.DataFrame(train_data)
val_data = pd.DataFrame(val_data)

start = past + future
end = start + train_split

x_train = train_data[[i for i in range(5)]].values
y_train = features.iloc[start:end][[titles[0]]] #change here the target training
print('training size:', len(x_train))

sequence_length = int(past / step)
print('window size:', sequence_length)

dataset_train = keras.preprocessing.timeseries_dataset_from_array(
    x_train,
    y_train,
    sequence_length=sequence_length,
    sampling_rate=step,
    batch_size=batch_size,
)

############################ SECTION: Prepare for training ############################


x_end = len(val_data) - past - future

label_start = train_split + past + future

x_val = val_data.iloc[:x_end][[i for i in range(5)]].values
y_val = features.iloc[label_start:][[titles[0]]] #change here the target training

dataset_val = keras.preprocessing.timeseries_dataset_from_array(
    x_val,
    y_val,
    sequence_length=sequence_length,
    sampling_rate=step,
    batch_size=batch_size,
)

for batch in dataset_train.take(1):
    inputs, targets = batch

print("Input shape:", inputs.numpy().shape)
print("Target shape:", targets.numpy().shape)

inputs = keras.layers.Input(shape=(inputs.shape[1], inputs.shape[2]))
lstm_out = keras.layers.LSTM(32)(inputs)
outputs = keras.layers.Dense(1)(lstm_out)

model = keras.Model(inputs=inputs, outputs=outputs)
model.compile(optimizer=keras.optimizers.Adam(learning_rate=learning_rate), loss='mse')
model.summary()

epochs = 10

path_checkpoint = "model_checkpoint.h5"
es_callback = keras.callbacks.EarlyStopping(monitor="val_loss", min_delta=0, patience=5)

modelckpt_callback = keras.callbacks.ModelCheckpoint(
    monitor="val_loss",
    filepath=path_checkpoint,
    verbose=1,
    save_weights_only=True,
    save_best_only=True,
)

############################ SECTION: Training and visualising the loss ############################


history = model.fit(
    dataset_train,
    epochs=epochs,
    validation_data=dataset_val,
    callbacks=[es_callback, modelckpt_callback],
)

def visualize_loss(history, title):
    loss = history.history["loss"]
    val_loss = history.history["val_loss"]
    epochs = range(len(loss))
    plt.figure()
    plt.plot(epochs, loss, "b", label="Training loss")
    plt.plot(epochs, val_loss, "r", label="Validation loss")
    plt.title(title)
    plt.xlabel("Epochs")
    plt.ylabel("Loss")
    plt.legend()
    plt.show()
    print("Final val loss: ", val_loss)

visualize_loss(history, "Training and Validation Loss")

############################ SECTION: Plot examples of future predictions ############################


dataset_test = keras.preprocessing.timeseries_dataset_from_array(
    x_val,
    y_val,
    sequence_length=sequence_length,
    sequence_stride=int(sequence_length * 6),
    sampling_rate=step,
    batch_size=1,
)

def show_plot(plot_data, delta, title):
    labels = ["History", "True Future", "Model Prediction"]
    marker = [".-", "rx", "go"]
    time_steps = list(range(-(plot_data[0].shape[0]), 0))
    if delta:
        future = delta
    else:
        future = 0
    plt.title(title)
    for i, val in enumerate(plot_data):
        if i:
            plt.plot(future, plot_data[i], marker[i], markersize=10, label=labels[i])
        else:
            plt.plot(time_steps, plot_data[i].flatten(), marker[i], label=labels[i])
    plt.legend()
    plt.xlim([time_steps[0], (future + 5) * 2])
    plt.xlabel("Time-Step")
    plt.show()
    return 

for x, y in dataset_test.take(5):
    prediction = model.predict(x)
    prediction = prediction[0]
    print('prediction:', prediction)
    print('truth:', y[0].numpy())
    show_plot(
        [x[0][:, 1].numpy(), y[0], prediction],
        12,
        "Single Step Prediction",
    )

############################ SECTION: Model conversion to TFLite ############################

converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.target_spec.supported_ops = [
tf.lite.OpsSet.TFLITE_BUILTINS, tf.lite.OpsSet.SELECT_TF_OPS
]
converter._experimental_lower_tensor_list_ops = True

tflite_model = converter.convert()

with open('plantemoji.tflite', 'wb') as f:
  f.write(tflite_model)


batch_size = 1
model.input.set_shape((batch_size,) + model.input.shape[1:])
model.summary()


# Our representative dataset is the same as the training dataset,
# but the batch size must now be 1
dataset_repr = keras.preprocessing.timeseries_dataset_from_array(
    x_train,
    y_train,
    sequence_length=sequence_length,
    sampling_rate=step,
    batch_size=batch_size,
)

def representative_data_gen():
  # To ensure full coverage of possible inputs, we use the whole train set
  for input_data, _ in dataset_repr.take(int(len(x_train))):
    input_data = tf.cast(input_data, dtype=tf.float32)
    yield [input_data]
    

############################ SECTION: Model quantization ############################

converter = tf.lite.TFLiteConverter.from_keras_model(model)
# This enables quantization
converter.optimizations = [tf.lite.Optimize.DEFAULT]
# This sets the representative dataset for quantization
converter.representative_dataset = representative_data_gen
# This ensures that if any ops can't be quantized, the converter throws an error
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
# For full integer quantization, though supported types defaults to int8 only, we explicitly declare it for clarity
converter.target_spec.supported_types = [tf.int8]
# These set the input and output tensors to int8
converter.inference_input_type = tf.uint8
converter.inference_output_type = tf.uint8
tflite_model_quant = converter.convert()

with open('plantemoji_quant.tflite', 'wb') as f:
  f.write(tflite_model_quant)
  

############################ SECTION: Testing the quantized model ############################

def set_input_tensor(interpreter, input):
  input_details = interpreter.get_input_details()[0]
  tensor_index = input_details['index']
  input_tensor = interpreter.tensor(tensor_index)()
  # Inputs for the TFLite model must be uint8, so we quantize our input data.
  scale, zero_point = input_details['quantization']
  quantized_input = np.uint8(input / scale + zero_point)
  input_tensor[:, :, :] = quantized_input

def predict_quan(interpreter, input):
  set_input_tensor(interpreter, input)
  interpreter.invoke()
  output_details = interpreter.get_output_details()[0]
  output = interpreter.get_tensor(output_details['index'])
  # Outputs from the TFLite model are uint8, so we dequantize the results:
  scale, zero_point = output_details['quantization']
  output = scale * (output - zero_point)
  return output

interpreter = tf.lite.Interpreter('plantemoji_quant.tflite')
interpreter.allocate_tensors()

for x, y in dataset_test.take(5):
  prediction = predict_quan(interpreter, x)
  print('prediction:', prediction[0])
  print('truth:', y[0].numpy())
  show_plot(
      [x[0][:, 1].numpy(), y[0], prediction[0]],
      12,
      "Single Step Prediction (TF Lite)",
  )