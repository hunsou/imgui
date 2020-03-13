#!/bin/bash
git rm -r --cached .
git add .
git commit -m "`date +'%Y-%m-%d %H:%M:%S'`"
#git tag -a v1.0.`date +'%Y.%m.%d.%H.%M.%S'` -m "`date +'%Y-%m-%d %H:%M:%S'`"
#git push origin master --tags
git push origin master