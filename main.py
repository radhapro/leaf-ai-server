 from fastapi import FastAPI, File, UploadFile
from ultralytics import YOLO
from PIL import Image
import io

app = FastAPI()

# YOLO model load
model = YOLO("best.pt")   # same folder me hona chahiye

@app.get("/")
def home():
    return {"message": "Leaf Detection Server Running"}

@app.post("/predict")
async def predict(file: UploadFile = File(...)):

    contents = await file.read()
    image = Image.open(io.BytesIO(contents))

    results = model(image)

    leaf_status = "Problem"

    for r in results:
        if len(r.boxes) > 0:
            leaf_status = "Leaf OK"

    return {
        "leaf_status": leaf_status
    }
