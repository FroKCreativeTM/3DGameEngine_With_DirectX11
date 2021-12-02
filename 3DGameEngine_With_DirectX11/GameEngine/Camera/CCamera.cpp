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
    // lookatpos�� ī�޶��� ��ġ�� �� �� ����!
    if (lookAtPos.x == this->m_pos.x &&
        lookAtPos.y == this->m_pos.y &&
        lookAtPos.z == this->m_pos.z)
    {
        return;
    }

    lookAtPos.x = this->m_pos.x - lookAtPos.x;
    lookAtPos.y = this->m_pos.y - lookAtPos.y;
    lookAtPos.z = this->m_pos.z - lookAtPos.z;

    // ��ġ�� �����Ѵ�.
    float pitch = 0.0f;
    if (lookAtPos.y != 0.0f)
    {
        const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
        // ��ź��Ʈ�� ������ ���ϱ� ���� �뵵
        pitch = atan(lookAtPos.y / distance);
    }

    float yaw = 0.0f;
    if (lookAtPos.x != 0.0f)
    {
        // ��ź��Ʈ�� ������ ���ϱ� ���� �뵵
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
    // ī�޶��� ȸ�� ����� ����Ѵ�. (��, ��ġ, ��� ������)
    XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z);

    // ���� ���� ���� ���� ���Ϳ�
    // ���� ȸ�� ������ �̿��ؼ� 
    // ī�޶��� ���� ���͸� ���̽��� �� ī�޶��� Ÿ�� ���͸� ���Ѵ�.
    XMVECTOR cameraTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, cameraRotationMatrix);

    // �׸��� ���� ī�޶��� ��ġ�� ������ ��Ų��.
    cameraTarget += this->m_posVector;

    // �׸��� ���� ȸ�� ���¸� �������� up���͸� ����Ѵ�.
    XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, cameraRotationMatrix);

    // �ٽ� �� ����� �����Ѵ�.
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
