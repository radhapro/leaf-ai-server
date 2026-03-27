from fastapi import FastAPI, File, UploadFile, HTTPException
from fastapi.responses import JSONResponse
import uvicorn
import io
from PIL import Image
import numpy as np       # <<<--- YE LINE ADD KARNI HAI
import tensorflow as tf  # <<<--- YE LINE ADD KARNI HAI

app = FastAPI()

# TensorFlow model load karna
try:
    model = tf.keras.models.load_model('plant_disease_model.h5')
    print("AI Model loaded successfully!")
except Exception as e:
    print(f"Error loading AI model: {e}")
    model = None 

# Model ke classes define karo (ye 'train_model.py' ke output se match karna chahiye)
# 'Classes: {'Diseased': 0, 'Healthy': 1}' ke hisaab se:
class_names = ['Diseased', 'Healthy'] 

@app.get("/")
async def read_root():
    return {"message": "Plant Disease Detection Server is LIVE!"}

@app.post("/predict")
async def predict_image(file: UploadFile = File(...)):
    if model is None:
        raise HTTPException(status_code=500, detail="AI Model not loaded.")

    try:
        # Image ko read karo
        image_data = await file.read()
        image = Image.open(io.BytesIO(image_data))

        # Image ko model ke input size par resize aur preprocess karo
        image = image.resize((150, 150)) # Model ne 150x150 images par train kiya tha
        image_array = np.array(image) / 255.0 # Pixels ko 0-1 range mein scale karna
        image_array = np.expand_dims(image_array, axis=0) # Batch dimension add karna

        # Prediction karo
        predictions = model.predict(image_array)
        
        # Sigmoid output (0 se 1 ke beech) ko class mein badlo
        # Agar prediction 0.5 se kam hai toh 'Diseased', nahi toh 'Healthy'
        predicted_class_index = (predictions > 0.5).astype(int)[0][0]
        predicted_class_name = class_names[predicted_class_index]
        
        confidence = float(predictions[0][0]) # Healthy hone ki confidence

        # JSON response
        return JSONResponse(content={
            "status": predicted_class_name,
            "confidence": confidence # Healthy hone ki probability
        })

    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Prediction failed: {e}")