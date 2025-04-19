import joblib
import polars as pl
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split

df = pl.read_csv("sensor_log.csv")
X = df.select(["ultraL", "ultraR", "irL", "irR", "temp"]).to_numpy()
y = df["label"].to_numpy()

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2)
model = RandomForestClassifier()
model.fit(X_train, y_train)

print("Accuracy:", model.score(X_test, y_test))
joblib.dump(model, "rf_model.pkl")
