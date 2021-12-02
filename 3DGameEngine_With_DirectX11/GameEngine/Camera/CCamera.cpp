#include "CCamera.h"

DEFINITION_SINGLE(CCamera);

using namespace DirectX;

CCamera::CCamera()
{
}

CCamera::~CCamera() {}

void CCamera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
    float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
    this->m_projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const DirectX::XMMATRIX& CCamera::GetViewMatrix() const
{
    return this->m_viewMatrix;
}

const DirectX::XMMATRIX& CCamera::GetProjectionMatrix() const
{
    return this->m_projectionMatrix;
}

const DirectX::XMVECTOR& CCamera::GetPositionVector() const
{
    return this->m_posVector;
}

const DirectX::XMFLOAT3& CCamera::GetPositionFloat3() const
{
    return this->m_pos;
}

const DirectX::XMVECTOR& CCamera::GetRotationVector() const
{
    return this->m_rotVector;
}

const DirectX::XMFLOAT3& CCamera::GetRotationFloat3() const
{
    return this->m_rot;
}

const DirectX::XMVECTOR& CCamera::GetForwardVector() 
{
    return m_vecForward;
}

const DirectX::XMVECTOR& CCamera::GetLeftVector() 
{
    return m_vecLeft;
}

const DirectX::XMVECTOR& CCamera::GetRightVector() 
{
    return m_vecRight;
}

const DirectX::XMVECTOR& CCamera::GetBackwardVector() 
{
    return m_vecBackward;
}

void CCamera::SetPosition(const DirectX::XMVECTOR& pos)
{
    XMStoreFloat3(&this->m_pos, pos);
    this->m_posVector = pos;
    this->updateViewMatrix();
}

void CCamera::SetPosition(float x, float y, float z)
{
    this->m_pos = XMFLOAT3(x, y, z);
    this->m_posVector = XMLoadFloat3(&this->m_pos);
    this->updateViewMatrix();
}

void CCamera::AdjustPosition(const DirectX::XMVECTOR& pos)
{
    this->m_posVector += pos;
    XMStoreFloat3(&this->m_pos, this->m_posVector);
    this->updateViewMatrix();
}

void CCamera::AdjustPosition(float x, float y, float z)
{
    this->m_pos.x += x;
    this->m_pos.y += y;
    this->m_pos.z += z;
    this->m_posVector = XMLoadFloat3(&this->m_pos);
    this->updateViewMatrix();
}

void CCamera::SetRotation(const DirectX::XMVECTOR& rot)
{
    this->m_rotVector = rot;
    XMStoreFloat3(&this->m_rot, rot);
    this->updateViewMatrix();
}

void CCamera::SetRotation(float x, float y, float z)
{
    this->m_rot = XMFLOAT3(x, y, z);
    this->m_rotVector = XMLoadFloat3(&this->m_rot);
    this->updateViewMatrix();
}

void CCamera::AdjustRotation(const DirectX::XMVECTOR& rot)
{
    this->m_rotVector += rot;
    XMStoreFloat3(&this->m_rot, this->m_rotVector);
    this->updateViewMatrix();
}

void CCamera::AdjustRotation(float x, float y, float z)
{
    this->m_rot.x += x;
    this->m_rot.y += y;
    this->m_rot.z += z;
    this->m_rotVector = XMLoadFloat3(&this->m_rot);
    this->updateViewMatrix();
}

void CCamera::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
{
    // lookatpos는 카메라의 위치가 될 수 없다!
    if (lookAtPos.x == this->m_pos.x &&
        lookAtPos.y == this->m_pos.y &&
        lookAtPos.z == this->m_pos.z)
    {
        return;
    }

    lookAtPos.x = this->m_pos.x - lookAtPos.x;
    lookAtPos.y = this->m_pos.y - lookAtPos.y;
    lookAtPos.z = this->m_pos.z - lookAtPos.z;

    // 피치를 조절한다.
    float pitch = 0.0f;
    if (lookAtPos.y != 0.0f)
    {
        const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
        // 역탄젠트는 각도를 구하기 위한 용도
        pitch = atan(lookAtPos.y / distance);
    }

    float yaw = 0.0f;
    if (lookAtPos.x != 0.0f)
    {
        // 역탄젠트는 각도를 구하기 위한 용도
        yaw = atan(lookAtPos.x / lookAtPos.z);
    }
    if (lookAtPos.z > 0)
    {
        yaw += XM_PI;
    }

    this->SetRotation(pitch, yaw, 0.0f);
}

void CCamera::updateViewMatrix()
{
    // 카메라의 회전 행렬을 계산한다. (롤, 피치, 요우 순으로)
    XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z);

    // 이후 원래 정면 상태 벡터와
    // 현재 회전 각도를 이용해서 
    // 카메라의 전방 벡터를 베이스로 한 카메라의 타겟 벡터를 구한다.
    XMVECTOR cameraTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, cameraRotationMatrix);

    // 그리고 현재 카메라의 위치에 적용을 시킨다.
    cameraTarget += this->m_posVector;

    // 그리고 현재 회전 상태를 바탕으로 up벡터를 계산한다.
    XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, cameraRotationMatrix);

    // 다시 뷰 행렬을 설정한다.
    this->m_viewMatrix = XMMatrixLookAtLH(this->m_posVector, cameraTarget, upDir);

    XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->m_rot.y, 0.0f);
    this->m_vecForward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
    this->m_vecBackward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
    this->m_vecLeft = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
    this->m_vecRight = XMVector3TransformCoord(this->DEFAULT_RGITHJ_VECTOR, vecRotationMatrix);
}

bool CCamera::Init()
{
    this->m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    this->m_posVector = XMLoadFloat3(&m_pos);
    this->m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    this->m_rotVector = XMLoadFloat3(&m_pos);
    this->updateViewMatrix();
    return true;
}
