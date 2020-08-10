@echo off

python updateVsProj.py -p pthread -c dll.projCfg.json -o ./
python updateVsProj.py -p pthread_lib -c lib.projCfg.json -o ./
python updateVsProj.py -p test -c test.projCfg.json -o ./

pause