import joblib
from micromlgen import port

model = joblib.load("rf_model.pkl")
with open("Model.h", "w") as f:
    f.write(port(model, classmap={0: "misaligned", 1: "aligned"}))
