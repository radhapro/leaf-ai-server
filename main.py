from fastapi import FastAPI, File, UploadFile
from PIL import Image
import io

app = FastAPI()

@app.get("/")
def home():
    return {"message": "Leaf Server Running"}

@app.post("/predict")
async def predict(file: UploadFile = File(...)):

    contents = await file.read()

    image = Image.open(io.BytesIO(contents))

    # Dummy logic (test only)
    # Later YOLO add karenge

    leaf_status = "Leaf OK"

    return {
        "leaf_status": leaf_status
    }