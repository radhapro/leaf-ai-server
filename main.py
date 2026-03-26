from fastapi import FastAPI, File, UploadFile
from fastapi.responses import JSONResponse

app = FastAPI()

@app.get("/")
def read_root():
    return {"message": "Leaf Disease Server is LIVE!"}

@app.post("/predict")
async def predict_leaf(file: UploadFile = File(...)):
    # Abhi hum AI use nahi kar rahe, toh dummy result bhej rahe hain
    # Aage chal kar yahan photo read karke AI model lagayenge
    
    # Man lo humne check kar liya aur patta beemar hai
    fake_result = "Diseased" 
    
    print(f"Photo aayi hai: {file.filename}")
    
    return JSONResponse(content={"status": fake_result})
