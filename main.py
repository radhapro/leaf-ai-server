from fastapi import FastAPI, File, UploadFile, HTTPException
from PIL import Image
import io
import os # Ye naya import hai
import requests # Ye naya import hai

app = FastAPI()

@app.get("/")
def home():
    return {"message": "Leaf Server Running"}

@app.post("/predict")
async def predict(file: UploadFile = File(...)):
    try:
        contents = await file.read()
        
        # Check if contents are empty
        if not contents:
            raise HTTPException(status_code=400, detail="No image data received.")

        # Image ko PIL (Pillow) se open karne ki koshish karo
        # Agar ye image nahi hai to error aa sakta hai
        image = Image.open(io.BytesIO(contents))
        
        # Optional: Save the received image for debugging
        # with open("received_image.jpg", "wb") as f:
        #     f.write(contents)
        # print("Image saved as received_image.jpg")

        # Yahan tumhari YOLO logic aayegi
        # Abhi ke liye, dummy logic
        leaf_status = "Leaf OK"
        print(f"Image received and processed. Dummy status: {leaf_status}") # Server logs mein dikhega

        return {
            "leaf_status": leaf_status,
            "image_format": image.format, # Image ka format bhi bhejo
            "image_size": f"{image.width}x{image.height}" # Image ka size bhi bhejo
        }

    except HTTPException as e:
        print(f"HTTP Exception: {e.detail}")
        raise e
    except Exception as e:
        print(f"Error processing image: {e}")
        raise HTTPException(status_code=500, detail=f"Error processing image: {e}")
