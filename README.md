
# Linux-Shell

### 구현 기능

#### cd
	* cd명령어와 디렉토리의 이름을 쉘에 입력 시 해당 디렉토리로 이동한다.
	* chdir() 함수 사용

####  exit
	* exit 명령어를 쉘에 입력 시 쉘을 종료한다.

####  background
	* 명령어 뒤에 “&”가 붙으면 백그라운드로 실행한다.
	* 명령어의 마지막 char이 “&”일 경우 변수 background를 1로 세팅.
	* fork()를 통해 자식 프로세스를 생성할 때, background가 1로 세팅 되어 있으면 자식 프로세스를 wait하지 않는다.
	* foreground일 경우 자식 프로세스가 종료될 때까지 wait 한다.

####  signal
	* SIGCHLD, SIGINT, SIGQUIT, SIGTSTP 처리
	* signal() 함수 사용

####  redirection
	* “>” 을 입력받게 될 경우 red_out 함수를 통해 텍스트 파일을 생성 후, 파일에 키보드 입력이 들어가게 함.
	* “<” 을 입력받게 될 경우 red_in 함수를 통해 입력받은 파일의 내용을 스크린으로 읽어온다.

####  pipe

