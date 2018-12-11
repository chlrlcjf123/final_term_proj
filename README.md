# 오픈소스 리눅스 프로그래밍 기말 과제


## 프로젝트 제목

화이트리스트 기반 IoT 보안 SW

## 1. 시스템구성

### 시스템 구성도
   ![default](https://user-images.githubusercontent.com/8528932/49825359-8a92dd00-fdc7-11e8-9519-c24e41e1e75e.png)

## 2. 구현 계획

### 구현 기능
   ![default](https://user-images.githubusercontent.com/8528932/49825479-e0678500-fdc7-11e8-90eb-4c58ddf2132a.png)

### 적용 기술
1. Controller와 Agent간 통신 

    리눅스 소켓 프로그래밍을 통해 네트워크 연결을 개발하며 아래와 같은 구조로 통신한다.
    
       ┌────────────┐                                             ┌──────┐
       │ Controller │ <-------------- (TCP 통신) -------------->  │ Agent │
       └────────────┘                                             └──────┘

2. Agent 기능 설명
    1. 허용되지 않은 IP/Port 차단 기능

         ![ip_port](https://user-images.githubusercontent.com/8528932/49825547-0f7df680-fdc8-11e8-8d13-10984ea45057.png)

         Message Queue에서 추출된 Rule을 배열로 된 Rule Table에 저장한다. (Rule Table은 IoT 장비의 하드웨어적 성능을 고려하여 특정 숫자만큼으로 배열의 크기 설정)
         리눅스의 기본 방화벽에 받은 Rule의 IP/Port를 설정한다.
         
     2. 허용되지 않은 Process 차단기능
     
        ![process](https://user-images.githubusercontent.com/8528932/49825602-31777900-fdc8-11e8-9a72-faa10ac3e25a.png)
        
        Process를 차단하기 위해 사용하는 LSM(Linux Security Module)의 경우 Capability, SELinux, AppArmor, smack들의 기법들은 모두 LSM을 기반으로 하고 있을 정도이다. 하지만 특정 기법(ex. SELinux)의 API를 이용해서 기능을 제공하면 특정 기법이 설치되지 않은 환경이나 복잡한 설정 때문에 사용하지 않는 경우 동작하지 않기 때문에 LSM을 기반으로 SELinux와 같은 모듈을 만드는 것을 목적으로 한다.
        
        자료 참조 : https://ssup2.github.io/theory_analysis/Linux_LSM/

3. Agent 내부 적용 기술

    pthread()와 Message Queue 활용하여 Agent의 기능들을 구현한다.
    
    1. pthread를 사용한 Thread 생성
    
         ![thread](https://user-images.githubusercontent.com/8528932/49825716-6f749d00-fdc8-11e8-9091-c946d03567df.png)
    
    2. Message Queue를 사용하여 변경된 Rule 확인 기능 생성
     
         ![messagequeue](https://user-images.githubusercontent.com/8528932/49825798-a21e9580-fdc8-11e8-8b1f-4b72a68e11c7.png)
     
         위의 그림을 보면 변경된 Rule을 확인하는 쓰레드에서 받은 데이터를 Message Queue에 넣는것을 볼 수 있다. 물론 전역변수를 이용하여 Thread간 공유하는 메모리 영역을 사용하여도 되나 Controller에서 전송하는 Rule의 개수는 가변적이기 때문에 위의 방식(전역변수를 이용하는 방식)을 사용할 경우 Linked List의 자료구조가 필요하다.
     
         하지만 Message Queue는 원하는 구현 방식인 구조체 형태의 데이터 공유 구조를 사용하며 따라서 Linked List를 코드로 구현하지 않고도 같은 기능을 하는 프로그램의 제작이 가능할 것이라고 판단하였다.
         
         Message Queue에서 사용할 데이터 구조 형식 :

             struct { 
               long data_type; /* data_type 값이 1이면 IP/Port와 관련된 Rule,
                               data_type 값이 2이면 Process와 관련된 Rule로
                               각각 쓰레드에서는 data_type 값을 체크해서 원하는 데이터만 가져간다.*/
               char data_buff[BUFF_SIZE]; /* IP/Port나 Process의 Rule이 담길 변수 */  
             }

### 동작 과정(FlowChart)

  ![flowchart](https://user-images.githubusercontent.com/8528932/49825844-bf536400-fdc8-11e8-8575-b3bd5ae7bdee.png)
