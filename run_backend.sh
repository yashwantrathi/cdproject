#!/bin/bash
cd web/backend

echo "Installing requirements..."
pip3 install --break-system-packages -r requirements.txt

echo "Starting Flask server..."
python3 app.py
