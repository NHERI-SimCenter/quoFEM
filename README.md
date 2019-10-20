# quoFEM

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3466061.svg)](https://doi.org/10.5281/zenodo.3466061)


This is a SimCenter research application. It's purpose is to provide
an application that will allow users to perform uncertainty
quantification and optimization methods utliizng existing finite
element applictions.

It will run the computations locally utilizing laptop/desktop or
remotely utilizing the computational resources made at tt TACC made
available through DesignSafe-ci.

Please visit the [quoFEM Research Tool webpage](https://simcenter.designsafe-ci.org/research-tools/quofem-application/)
for more resources related to this tool. Additionally, this page
provides more information on the NHERI SimCenter, including other SimCenter
applications, FAQ, and how to collaborate.

### How to Build
##### 1. Download this repo.

##### 2. Download the SimCenterCommon repo: https://github.com/NHERI-SimCenter/SimCenterCommon

Place the SimCenterCommon Repo in the same directory that you placed the quoFEM repo.

##### 3. Install Qt: https://www.qt.io/

Qt is free for open source developers. Download it and start the Qt Creator application. From Qt Creator open the quoFEM.pro file located in the directory the quoFEM repo was downloaded into and select build to build it. For detailed instructions on using Qt, browse their website.

##### 4. Copy the parseDakota.py file located in localApp directory to the folder containg the application. Mac users note this is not the folder containing quoFEM.app, but a folder inside that folder quoFEM.app/Contents/Mac
.
### Acknowledgement

This material is based upon work supported by the National Science Foundation under Grant No. 1612843.
