@echo off
git rm -r --cached .
git add .
git commit -m "%date:~6,4%-%date:~0,2%-%date:~3,2% %time:~0,8%"
::git tag -a v1.1.7 -m ""
rem git push origin master --tags
git push origin master