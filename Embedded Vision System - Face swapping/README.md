# Face_swapping using opencv and python
[[Python 코드보기]](https://github.com/sangwoo24/Main-Project-in-KPU/blob/master/Embedded%20Vision%20System%20-%20Face%20swapping/realtime_face_swapping.py)

Opencv와 파이썬을 이용한 얼굴 바꾸기 프로그램입니다.

Tkinter 를 사용해 인터페이스를 구현했고, 원하는 사진을 선택하면 그 사진과 바꿀 수 있게 설계했습니다.

초기화면에는 얼굴바꾸기 옵션과 사진추가하기 옵션을 두어 자기가 원하는 사진을 추가시켜 그 사진과도 얼굴바꾸기를

할 수 있게 구성했습니다. 얼굴바꾸기로 들어가면 예시로 어떻게 바뀌는지 보여주고, 얼굴들의 List를 보여줌으로써 현재 저장돼있는

사진을 통해 어떤 얼굴과 바꿀 수 있는지를 보여줍니다. 보는 방법은 List에 존재하는 이름을 더블클릭하면 사진으로 보여주고,

그 이름을 밑에 입력칸에 복사하거나 입력하고 Start를 누르면 잠시 후 자신과 사진의 얼굴이 바뀌어있는 것을 볼 수 있습니다.

영상처리 과목 텀 프로젝트로 수행하였는데, 어떤 프로젝트를 수행할지 고민하다가 딥러닝을 활용하여 재밌게 프로그램 할 수 있는

것을 찾다보니 이 프로젝트를 선정하였습니다.



materials
---------

1. dlib 
   이 프로젝트는 딥러닝 기반으로 사람의 얼굴을 인식해 68개의 Point를 생성하고 그 점들을 활용하여 얼굴을 바꾸는 프로그램
   
   이기 때문에 딥러닝을 사용하기 위해서는 dlib가 필요합니다. 

2. Tkinter
   인터페이스를 구현하고 사진을 캡쳐하기위해 캠 영상이 프레임 안에 들어가있어야 합니다. 그러기 위해선 Tkinter가 필요.
   

Process
-------

1. 사용자가 원하는 사진을 입력하고 start를 누르면 landmark 를 이용해 선택한 사진에서 68개의 Point를 구합니다.

2. 각 Point를 3개씩 잡아 삼각형을 만들어줍니다.Delaunay triangulation(들로네 삼각분할)을 사용하기 위함.
   
   - 삼각분할을 사용하는 이유?
     
     선택한 이미지에서 얼굴을 잘라내서 크기와 원근법이 다른 대상 이미지에 넣을 수는 없습니다. 따라서 크기와 원근법
     
     이 다른 영상이나 사진에도 맞춰주기 위해서 삼각분할을 사용합니다. 바꾸려는 사진과의 삼각형과 일치시키면 문제가 
     
     발생하지 않기 때문입니다.
     
3. 사용자가 선택한 사진의 삼각분할이 끝난다면 while(1) 무한루프를 통해 사용자의 얼굴을 cam를 통해서 frame으로 입력
  
   받습니다.
   
4. 사용자의 얼굴을 frame으로 받는 동시에 사진에서 했던 작업이랑 동일하게 landmark를 통해 68개의 point를 잡고, 
 
   삼각분할을 사용하여 분할 해줍니다.
   
5. 사진과 영상 모두 삼각분할이 이루어지면 같은 point위치에 있는 삼각형끼리 서로 대조시켜 바꿔줍니다.
 
   여기서 사진과 영상의 얼굴색을 맞추기 위해 내장함수인 seamlessClone 를 사용합니다.
   
6. 모든 작업이 끝나면 출력되는 영상으로 자신과 사진의 얼굴이 바뀐 모습을 볼 수 있습니다.
<br>


Screenshot
--------

제 노트북 카메라가 고장나서 흑백밖에 안나오는점 죄송합니다. 다른 노트북으로 할 시 컬러화면으로 잘 나오니 착오가 없길 바라겠습니다.

Start
-
<img width = "18%" src = "https://user-images.githubusercontent.com/56511253/71498522-0ca15700-28a0-11ea-8070-4aac09ca76ca.PNG">
<br><br>

1.얼굴바꾸기
-
<img width = "18%" src = "https://user-images.githubusercontent.com/56511253/71498523-0d39ed80-28a0-11ea-88f6-accc25b28b0e.PNG">
<br><br>

2.리스트보기
-
<img width = "18%" src = "https://user-images.githubusercontent.com/56511253/71498527-0dd28400-28a0-11ea-87df-78d2ca818419.PNG">
<br><br>

3.스냅샷
-
<img width = "18%" src = "https://user-images.githubusercontent.com/56511253/71498737-d44e4880-28a0-11ea-825a-62a3d2a29245.PNG">
<br><br>

4.결과
-
<img width = "18%" src = "https://user-images.githubusercontent.com/56511253/71498529-1034de00-28a0-11ea-8019-0d12ab7731b3.PNG">
<br>


