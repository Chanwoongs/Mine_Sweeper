Mine Sweeper Log

저번에 고치지 못했던 오류 
	많은 마우스 클릭 후 게임이 종료 될 때 ~Map()에서 delete 에러 발생
	         (에러가 날 때도 있고 안날 때도 있음)
디버그
	소멸자 동적할당 소멸 순서 변경
	main에서 map, mine 생성자 소멸자만 호출 후 에러 여부 확인
	게임 로직에서 size 범위를 벗어나는 부분 if 문으로 예외 처리
	메모리 해제 전 모든 동적 할당 요소 초기화
	main 함수의 game loop의 모든 함수 확인 후 Update 함수에 문제가 있는 것을 확인
오류 발견 
	setNearBlocks 함수에서 선택한 블럭 주위 9개를 모두 처리하는 과정에서 nearMines 메모리 초과 발생
오류 수정
	1. 하나 씩 Block을 여는 함수로 변경 후 오류 안남 확인 (확인 후 빈 곳을 연속으로 여는 기능 추가, 기능 추가 후 오류 재발생)
	2. nearMines의 값을 0이 아닌 -1로 초기화를 하는 예외 처리 후 nearMines 오류 없음 확인 -> isOpened에서 다시 메모리 초과 발생
	3. checkNearMines의 함수에서 isOpened가 false일 때만 true로 바꾸는 예외처리 후 오류 없음 확인 
Mine Sweeper 수정내용

bool* isWall 추가 : 현재 블럭이 더 열릴수 있는지 없는지 확인
setNearBlocks() : 원래 자신 포함 주위 9개 블럭을 여는 함수 -> 재귀를 이용하여, 클릭한 곳이 빈곳 일 경우 계속해서 여는 기능 추가 
strncpy 대신 문자 대입으로 변경 (문자열이 아니기 때문에 문자열을 복사할 필요 X)
Input Class로 마우스 입력 받음
getMouseButtonDown() : 마우스 왼쪽 버튼이 눌릴 시 true 반환
getMousePosition() : 호출 될 시의 마우스 현재 좌표를 알려줌

