import tensorflow as tf
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, MaxPooling2D, Flatten, Dense, Dropout
import os
import matplotlib.pyplot as plt # Optional: for plotting training history

# Dataset ka base directory define karo
# Ye path adjust karna padega agar tumhara Plant_Disease_Dataset folder kahi aur hai
base_dir = './Plant_Disease_Dataset' 

# Image data generators (preprocessing aur data augmentation ke liye)
train_datagen = ImageDataGenerator(
    rescale=1./255, # Pixels ko 0-1 range mein scale karna
    rotation_range=20,
    width_shift_range=0.2,
    height_shift_range=0.2,
    shear_range=0.2,
    zoom_range=0.2,
    horizontal_flip=True,
    fill_mode='nearest',
    validation_split=0.2 # 20% data validation ke liye
)

# Training data generator
train_generator = train_datagen.flow_from_directory(
    base_dir,
    target_size=(150, 150), # Saari images ko 150x150 pixels mein resize karega
    batch_size=32,
    class_mode='binary', # Kyunki do hi classes hain: Healthy ya Diseased
    subset='training'
)

# Validation data generator (data augmentation nahi hogi isme)
validation_generator = train_datagen.flow_from_directory(
    base_dir,
    target_size=(150, 150),
    batch_size=32,
    class_mode='binary',
    subset='validation'
)

# Check classes and labels
print("Classes:", train_generator.class_indices)
# Output should be something like: {'Diseased': 0, 'Healthy': 1} or vice-versa

# Model architecture define karna
model = Sequential([
    Conv2D(32, (3, 3), activation='relu', input_shape=(150, 150, 3)),
    MaxPooling2D(2, 2),
    Conv2D(64, (3, 3), activation='relu'),
    MaxPooling2D(2, 2),
    Conv2D(128, (3, 3), activation='relu'),
    MaxPooling2D(2, 2),
    Flatten(),
    Dropout(0.5), # Overfitting rokne ke liye
    Dense(128, activation='relu'),
    Dense(1, activation='sigmoid') # Binary classification ke liye sigmoid (0 or 1)
])

# Model compile karna
model.compile(optimizer='adam',
              loss='binary_crossentropy',
              metrics=['accuracy'])

model.summary()

# Model training
epochs = 20 # Kitni baar model data ko dekhega
# Agar dataset chhota hai (tumhare case mein 30-30 pics), toh 20-30 epochs kaafi ho sakte hain.
# Agar result acche nahi aaye toh badha sakte hain.
history = model.fit(
    train_generator,
    steps_per_epoch=train_generator.samples // train_generator.batch_size,
    epochs=epochs,
    validation_data=validation_generator,
    validation_steps=validation_generator.samples // validation_generator.batch_size
)

# Model ko save karna
model_save_path = './plant_disease_model.h5' # Isi project folder mein save hoga
model.save(model_save_path)
print(f"Model successfully saved to: {model_save_path}")

# Optional: Training history plot karna
# Ye graph dikhayega ki accuracy aur loss kaise badle training ke dauran
plt.figure(figsize=(12, 4))
plt.subplot(1, 2, 1)
plt.plot(history.history['accuracy'], label='Training Accuracy')
plt.plot(history.history['val_accuracy'], label='Validation Accuracy')
plt.legend()
plt.title('Accuracy over Epochs')

plt.subplot(1, 2, 2)
plt.plot(history.history['loss'], label='Training Loss')
plt.plot(history.history['val_loss'], label='Validation Loss')
plt.legend()
plt.title('Loss over Epochs')
plt.show() # Plot dikhane ke liye