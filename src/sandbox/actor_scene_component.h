#pragma once
#include <memory>
#include <vector>

#include "actor_component.h"
#include "euler_angle_utils.h"

class SceneComponent : public ActorComponent
{
public:
    virtual void TickComponent(float deltaTime) override;

    bool AttachToComponent(SceneComponent* parent);     // ����ǰ������ӵ�ָ���ĸ����
    void DetachFromParent();                            // ����ǰ����Ӹ�����з���

    SceneComponent* GetAttachParent() const;            // ��ȡ��ǰ����ĸ����

    int GetNumChildrenComponents() const;               // ��ȡ��ǰ��������������
    SceneComponent* GetChildComponent(int ChildIndex) const;                // ��ȡָ�������������
    void GetChildrenComponents(std::vector<SceneComponent*>& Children);     // ��ȡ������������б�
    std::vector<SceneComponent*> GetAttachChildren();                       // ��ȡ���и��ӵ������

    [[nodiscard]]
    glm::mat4 GetComponentToWorld() const;              // ��ȡ����������������ϵ�ı任����
    void SetComponentToWorld(const glm::mat4& mat);     // ��������������������ϵ�ı任����
    glm::mat4 GetRelativeTransform() const;             // ��ȡ�������ڸ�����ı任����
    void SetRelativeTransform(const glm::mat4& mat);    // �����������ڸ�����ı任����

    void SetRelativeLocation(glm::vec3 relativeLocation);  // �������λ��
    glm::vec3 GetRelativeLocation() const;  // ��ȡ���λ��

    void SetRelativeRotation(glm::quat relativeRotation);  // ���������ת
    glm::quat GetRelativeRotation() const;  // ��ȡ�����ת

    void SetRelativeScale3d(glm::vec3 relativeScale3d);  // �����������
    glm::vec3 GetRelativeScale3d() const;  // ��ȡ�������

    void ParentTransformChanged();      // ������ı任�����ı�Ĵ����߼�
    void LocalToWorldChanged();         // ���ص���������ϵ�ı任��ϵ�����ı�Ĵ����߼�
    void RelativeTransformChanged();    // ��Ա任��ϵ�����ı�Ĵ����߼�
    void RelativeLocationChanged();     // ���λ�÷����ı�Ĵ����߼�
    void RelativeRotationChanged();     // �����ת�����ı�Ĵ����߼�
    void RelativeScale3dChanged();      // ������ŷ����ı�Ĵ����߼�

    glm::vec3 _relative_location;   // ���λ��
    glm::quat _relative_rotation;   // �����ת
    glm::vec3 _relative_scale3d;    // �������

    glm::vec3 _world_location_cache;    // ��������ϵ�е�λ�û���
    glm::quat _world_rotation_cache;    // ��������ϵ�е���ת����
    glm::vec3 _world_scale3d_cache;     // ��������ϵ�е����Ż���

    glm::mat4 _local_to_parent_cache;  // ����ڸ�����ı任���󻺴�
    glm::mat4 _local_to_world;  // �������������ϵ�ı任����

    SceneComponent* _attach_parent = nullptr;  // �����ָ��
    std::vector<std::unique_ptr<SceneComponent>> _attach_children;  // ������б�
};