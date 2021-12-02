### FroK' Game Engine With DirectX
3D를 제공하는 다이렉트X 공부 및 게임의 통신을 공부하기 위한 레포지토리입니다.

# 2021-11-24
WinApiGameEngine 프로젝트 (url : https://github.com/FroKCreativeTM/WinApiGameEngine) 를 참고하며, 렌더링 API는 DirectX를 사용합니다.
</br> 그리고 이를 이용해서 게임 엔진을 제작합니다.

- 윈도우 창 구현

# 2021-11-25
- DirectX 11 렌더링 잡기 완료</br>

- Device :  물리적인 그래픽스 장치에 대한 소프트웨어 컨트롤러 인터페이스, 자원을 할당하거나, 특징들이 지원되는지 체크한다.
- DeviceContext : 렌더링 상태를 세팅하고, 그래픽스 파이프라인에 자원을 바인딩하며, 렌더링 커맨드에 대해 처리한다.

- 셰이더 설정 및 PATH매니저, 그리고 Input 클래스의 싱글톤화

- 알아야 할 키워드 
</br></br>
1. DXGI_SWAP_CHAIN_DESC - 스왑 체인(렌더링 버퍼를 관리)하기 위한 구조체 타입</br>
2. 어뎁터가 무엇인가? - 내 그래픽 카드에 대한 정보를 가져오기 위한 데이터 타입</br>
3. Microsoft::WRL::ComPtr - 마이크로소프트의 ABI, 프로세스 간의 통신과 동적 오브젝트 생성이 가능해진다.</br>
4. ID3D11Device - DirectX 디바이스 그 자체, 렌더링할 때 옵션 전반을 관리하는 느낌으로 보면 될 듯</br>
5. ID3D11DeviceContext - 현재 디바이스 내 상황을 관리하기 위한 자료형 </br>

- 그래픽 렌더링에 필요한 것
1. Input 어셈블러(IA) : 첫 번째 목적은 응용 프로그램에서 제공한 정점 버퍼로부터 
정점 데이터를 다른 파이프라인 단계에서 사용할 프리미티브로 조립하는 것</br>
두 번째 목적은 시스템 생성 값(System-generated-Value)을 추가하는 것 (Complete)</br>
2. VERTEX SHADER - 정점들을 관리하기 위해서 존재하는 셰이더(Complete)</br>
3. RESTERIZER(Complete)</br>
4. PIXEL SHADER(Complete)</br>
5. OUTPUT MERGER - OMSetRenderTargers(Complete)</br>

#2021-11-26
- 정점 쉐이더 및 픽셀 쉐이더의 구조체화 및 그래픽스 클래스 수정

- Depth 버퍼와 Stencil 버퍼 설정
스텐실 버퍼는 깊이 버퍼, 백 버퍼와 마찬가지로 같은 크기를 가지는 오프 스크린 버퍼이다.
백버퍼의 일부 영역을 차단하여 그리지 않기 위해 사용되어지는 특수한 버퍼라고 생각하면 된다.
보통 거울이나 평면 그림자에 사용되며, 이 거울에 비치지 않는 물체를 차단하기 위해 있는 것이 바로 이 스텐실 버퍼!

- 텍스트 띄우기 완료(폰트 매니저 클래스 만들 예정)

# 2021-12-02
View는 Eye의 position 그리고 Lookat pos 그리고 Up Vector 이 3개의 vector의 x/y/z를 담당한다.
그리고 Projection은 FOV, Near/Far Z를 담당한다.