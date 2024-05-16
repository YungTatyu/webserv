FROM python:3.8

WORKDIR .

COPY requirements.txt .

RUN pip3 install --no-cache-dir -r requirements.txt

# ソースコードをコピー
COPY . .

CMD ["python3", "test/run_tests.py"]
