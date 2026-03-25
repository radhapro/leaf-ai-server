from fastapi import FastAPI, File, UploadFile, HTTPException
from PIL import Image
import io
import os
import requests
from ultralytics import YOLO # Naya import: YOLO library
import numpy as np # Naya import: numpy
import torch # Naya import: torch (ultralytics use karta hai)

app = FastAPI()

# YOLO Model ko load karna
# Ye model pehli baar run hone par download hoga (Render logs mein dikhega)
# yolo_model = YOLO('yolov8n.pt') # YOLOv8n nano model

# Model ko global variable banate hain aur pehli request pe load karte hain
# Taki Render server ke start hone par zyada delay na ho
yolo_model = None

def load_yolo_model():
    global yolo_model
    if yolo_model is None:
        print("Loading YOLOv8n model...")
        # 'yolov8n.pt' ko download karke load karega
        # model ko CPU par run karte hain Render free tier ke liye
        yolo_model = YOLO('yolov8n.pt') 
        print("YOLOv8n model loaded successfully.")
    return yolo_model

@app.get("/")
def home():
    return {"message": "Leaf Server Running"}

@app.post("/predict")
async def predict(file: UploadFile = File(...)):
    try:
        load_yolo_model() # Har request se pehle model load ya confirm loaded

        contents = await file.read()
        
        if not contents:
            raise HTTPException(status_code=400, detail="No image data received.")

        # Image ko PIL (Pillow) se open karo
        image = Image.open(io.BytesIO(contents))
        
        # PIL image ko numpy array mein convert karo jo YOLO ko chahiye
        # YOLO ko direct PIL image bhi de sakte hain, lekin numpy zyada robust hai
        img_array = np.array(image)

        # YOLO model se prediction karo
        # Ab hum actual detection results dekhenge
        print("Running YOLO inference...")
        # 'img_array' ko 'source' parameter mein dete hain
        results = yolo_model(img_array) # YOLO inference on the image
        print("YOLO inference complete.")

        # Results ko process karo
        leaf_status = "Leaf OK" # Default status
        detected_objects = []
        
        for r in results:
            boxes = r.boxes # Bounding boxes
            # Classes aur confidence scores
            for box in boxes:
                class_id = int(box.cls)
                confidence = float(box.conf)
                name = yolo_model.names[class_id] # Class ka naam
                
                detected_objects.append({
                    "class": name,
                    "confidence": round(confidence, 2),
                    "box": box.xyxy[0].tolist() # Bounding box coordinates
                })
                print(f"Detected: {name} with confidence {confidence:.2f}")
                
                # Example: Agar tumhe koi "diseased_leaf" ya "pest" jaisa class detect karna hai
                # to tum yahan conditional logic laga sakte ho
                # Abhi ke liye, sirf "Leaf OK" return kar raha hun agar koi problem detect na ho
                # Ya tum yahan decide kar sakte ho ki agar koi problem wala object detect hua to status change ho
                # if name == "diseased_leaf": # Replace with your actual class names
                #     leaf_status = "Leaf Diseased"
                #     break # Agar ek diseased leaf mil gaya to loop se bahar aa jao

        if not detected_objects:
            leaf_status = "No objects detected in image." # Agar kuch detect nahi hua

        return {
            "leaf_status": leaf_status, # Ab ye actual YOLO based hoga
            "detected_objects": detected_objects,
            "image_format": image.format,
            "image_size": f"{image.width}x{image.height}"
        }

    except HTTPException as e:
        print(f"HTTP Exception: {e.detail}")
        raise e
    except Exception as e:
        print(f"Error processing image: {e}")
        raise HTTPException(status_code=500, detail=f"Internal Server Error: {e}")
