#pragma once
#include <memory>
#include <vector>

#include "actor_component.h"
#include "euler_angle_utils.h"

class SceneComponent : public ActorComponent
{
public:
    virtual void TickComponent(float deltaTime) override;

    bool AttachToComponent(SceneComponent* parent);     // 将当前组件附加到指定的父组件
    void DetachFromParent();                            // 将当前组件从父组件中分离

    SceneComponent* GetAttachParent() const;            // 获取当前组件的父组件

    int GetNumChildrenComponents() const;               // 获取当前组件的子组件数量
    SceneComponent* GetChildComponent(int ChildIndex) const;                // 获取指定索引的子组件
    void GetChildrenComponents(std::vector<SceneComponent*>& Children);     // 获取所有子组件的列表
    std::vector<SceneComponent*> GetAttachChildren();                       // 获取所有附加的子组件

    [[nodiscard]]
    glm::mat4 GetComponentToWorld() const;              // 获取组件相对于世界坐标系的变换矩阵
    void SetComponentToWorld(const glm::mat4& mat);     // 设置组件相对于世界坐标系的变换矩阵
    glm::mat4 GetRelativeTransform() const;             // 获取组件相对于父组件的变换矩阵
    void SetRelativeTransform(const glm::mat4& mat);    // 设置组件相对于父组件的变换矩阵

    void SetRelativeLocation(glm::vec3 relativeLocation);  // 设置相对位置
    glm::vec3 GetRelativeLocation() const;  // 获取相对位置

    void SetRelativeRotation(glm::quat relativeRotation);  // 设置相对旋转
    glm::quat GetRelativeRotation() const;  // 获取相对旋转

    void SetRelativeScale3d(glm::vec3 relativeScale3d);  // 设置相对缩放
    glm::vec3 GetRelativeScale3d() const;  // 获取相对缩放

    void ParentTransformChanged();      // 父组件的变换发生改变的处理逻辑
    void LocalToWorldChanged();         // 本地到世界坐标系的变换关系发生改变的处理逻辑
    void RelativeTransformChanged();    // 相对变换关系发生改变的处理逻辑
    void RelativeLocationChanged();     // 相对位置发生改变的处理逻辑
    void RelativeRotationChanged();     // 相对旋转发生改变的处理逻辑
    void RelativeScale3dChanged();      // 相对缩放发生改变的处理逻辑

    glm::vec3 _relative_location;   // 相对位置
    glm::quat _relative_rotation;   // 相对旋转
    glm::vec3 _relative_scale3d;    // 相对缩放

    glm::vec3 _world_location_cache;    // 世界坐标系中的位置缓存
    glm::quat _world_rotation_cache;    // 世界坐标系中的旋转缓存
    glm::vec3 _world_scale3d_cache;     // 世界坐标系中的缩放缓存

    glm::mat4 _local_to_parent_cache;  // 相对于父组件的变换矩阵缓存
    glm::mat4 _local_to_world;  // 相对于世界坐标系的变换矩阵

    SceneComponent* _attach_parent = nullptr;  // 父组件指针
    std::vector<std::unique_ptr<SceneComponent>> _attach_children;  // 子组件列表
};