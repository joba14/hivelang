
#!/bin/sh


make

cd ./tests
python3 ./.tester.py --settings ./.settings.json

cd ./../
