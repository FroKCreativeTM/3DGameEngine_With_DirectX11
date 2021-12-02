#pragma once

#include "../Game.h"

class CCamera
{
public :
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const DirectX::XMMATRIX& GetViewMatrix() const;
	const DirectX::XMMATRIX& GetProjectionMatrix() const;

	const DirectX::XMVECTOR& GetPositionVector() const;
	const DirectX::XMFLOAT3& GetPositionFloat3() const;
	const DirectX::XMVECTOR& GetRotationVector() const;
	const DirectX::XMFLOAT3& GetRotationFloat3() const;

	const DirectX::XMVECTOR& GetForwardVector() ;
	const DirectX::XMVECTOR& GetLeftVector() ;
	const DirectX::XMVECTOR& GetRightVector() ;
	const DirectX::XMVECTOR& GetBackwardVector() ;

	void SetPosition(const DirectX::XMVECTOR& pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const DirectX::XMVECTOR& pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const DirectX::XMVECTOR& rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const DirectX::XMVECTOR& rot);
	void AdjustRotation(float x, float y, float z);
	void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);

private : 
	void updateViewMatrix();

	DirectX::XMVECTOR m_posVector;
	DirectX::XMVECTOR m_rotVector;
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_rot;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;

	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR	DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR	DEFAULT_RGITHJ_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);


	DirectX::XMVECTOR m_vecForward;
	DirectX::XMVECTOR m_vecLeft;
	DirectX::XMVECTOR m_vecRight;
	DirectX::XMVECTOR m_vecBackward;

public : 
	bool Init();

public :
	DECLARE_SINGLE(CCamera);
};

