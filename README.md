# 오픈소스 소프트웨어 제작

## License
This project is licensed under the [MIT License](LICENSE).
![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)

## 전류 센서를 활용한 가전제품의 전력 소비 분석

-본 보고서는 ACS 712 전류 센서와 ESP 32 마이크로컨트롤러를 사용하여, 가전제품의 사용 전류 패턴을 통계 기법을 이용하여 분석함  
-본 보고서 개발 범위는 전류 변동성, 분산, 범위 분석뿐만 아니라 이상치 탐지를 수행함   
-분석 결과, 통계적 및 시계열 기반의 분석을 통해 가전제품의 전류 사용 특성을 분류할 수 있었음  
-ESP 32에서의 프로그램 파일은 "ESP32 Program.ino"이고,    
-노트북에서의 프로그램 파일은 "Notebook Program.py"임   


 
## 전체 시퀀스 다이어그램
-이 시퀀스 다이어그램은 시스템 구성 요소 간의 통신을 보여줌

```mermaid
sequenceDiagram
    participant Laptop
    participant IpTime as WiFi Router (IpTime)
    participant ESP32
    participant ACS712
    participant Appliances as Home Appliances
    
    Laptop->>IpTime: Connects to WiFi
    ESP32->>IpTime: Connects to WiFi
    Appliances->>ACS712: Monitors current usage
    ACS712->>ESP32: Reads current data
    ESP32->>Laptop: Sends current data via WiFi

