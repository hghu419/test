在 Geant4 中，“从原理上”在 `SteppingAction` 里统计穿过某个体积（比如你的“虚拟层”）的粒子数，核心思路就是：

1. **检测边界跨越**
   每个粒子在运动过程中都会被划分成一系列的 step（步进）。每个 step 有一个“前点”（PreStepPoint）和“后点”（PostStepPoint），它们分别属于两个体积（或同一个）。当一个 step 的前点不在目标体积、而后点正好落在目标体积里，就意味着该粒子“进入”了目标体积一次。

2. **只计一次／多次**

   * 如果你只想按“粒子进入次数”计数，这种“边界跨越”的检测就足够了。
   * 如果你想统计“有多少**条**粒子轨迹”一旦进入就算一次，不重复累计，可在检测到跨越后记录该轨迹的 `TrackID`，并用 `std::set<G4int>` 只对新轨迹计数一次。

3. **实现步骤**

   * **获取前后点的逻辑体积指针**

     ```cpp
     auto preVol  = step->GetPreStepPoint()
                         ->GetTouchableHandle()
                         ->GetVolume()
                         ->GetLogicalVolume();
     auto postVol = step->GetPostStepPoint()
                         ->GetTouchableHandle()
                         ->GetVolume()
                         ->GetLogicalVolume();
     ```
   * **和目标体积 pointer 比较**

     ```cpp
     G4LogicalVolume* virtVol = fDet->GetVirtualLayerLogical();
     bool crossed = (preVol != virtVol) && (postVol == virtVol);
     ```
   * **调用事件级计数器**

     ```cpp
     if (crossed) {
       fEventAction->AddParticleCount();
     }
     ```

     这样，每次“跨入”就给当前事件的计数器加 1。

4. **为什么放在 SteppingAction？**

   * `SteppingAction` 对于每一步都可见，而且能够访问到 `StepPoint` 的体积信息；
   * 它被每个线程（或在单线程模式下的主线程）调用，配合你的 `EventAction`（线程私有），就能安全地在每个事件内累加。

5. **EventAction 如何输出**

   ```cpp
   void EventAction::EndOfEventAction(const G4Event* event) {
     G4cout << "[Event " << event->GetEventID() << "] 通过虚拟层的粒子数: "
            << fCount << G4endl;
   }
   ```

   `fCount` 在 `BeginOfEventAction()` 被清零，在 `AddParticleCount()` 中递增。

6. **完整示例**

   ```cpp
   // SteppingAction.hh
   class SteppingAction : public G4UserSteppingAction {
   public:
     SteppingAction(DetectorConstruction* det, EventAction* evt)
       : fDet(det), fEventAction(evt) {}
     void UserSteppingAction(const G4Step* step) override;
   private:
     DetectorConstruction* fDet;
     EventAction*          fEventAction;
   };

   // SteppingAction.cc
   void SteppingAction::UserSteppingAction(const G4Step* step) {
     // 1. 获取前后体积
     auto prePV = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
     auto postPV= step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
     if (!prePV || !postPV) return;  // 边界外保护
     auto preVol  = prePV->GetLogicalVolume();
     auto postVol = postPV->GetLogicalVolume();

     // 2. 目标虚拟层
     auto virtVol = fDet->GetVirtualLayerLogical();

     // 3. 边界跨越检测
     if (preVol != virtVol && postVol == virtVol) {
       // 4. 递增事件内计数
       fEventAction->AddParticleCount();
     }
   }
   ```

---

### 其他可选方法

* **使用敏感探测器（Sensitive Detector）**
  如果你已经在 `DetectorConstruction::ConstructSDandField()` 里给虚拟层绑定了多功能探测器（`G4MultiFunctionalDetector`），并用 `G4PSFlatSurfaceFlux` 或自定义 `G4VSensitiveDetector` 收集“粒子通量”或“命中数”，那么你也可以在 `EndOfEventAction()` 里通过 `G4AnalysisManager` 或 `G4SDManager` 查询该探测器在本事件内的记录条数。

* **避免重复计数**
  如果一个粒子多次来回穿过边界（比如反弹、二次散射），上面的方法会每次跨入都计数。如果你只想要**每个粒子**（每个 `TrackID`）最多计一次，可以在 `EventAction` 里维护一个 `std::set<G4int>`，在跨入时做：

  ```cpp
  G4int id = step->GetTrack()->GetTrackID();
  if (setOfIDs.insert(id).second) {
    // 新插入，说明第一次进入
    ++fCount;
  }
  ```

---

#### 小结

* **原理**：通过 `UserSteppingAction` 每步检查“前体积→后体积”的跨越来检测“粒子进入”事件；
* **调用**：检测到跨越就调用 `EventAction::AddParticleCount()`；
* **输出**：在 `EndOfEventAction()` 中输出该事件累积的 `fCount`；
* **可扩展**：可防重复、或使用敏感探测器／多功能探测器来统计；

按照上述思路，你就能在 `SteppingAction` 里从原理上、稳定地实现粒子数的读出与统计功能。


每次你在宏里执行

```shell
/run/beamOn N
```

实际上就是让 Geant4 产生 N 个事件（event），而每个事件里，`PrimaryGeneratorAction` 又按照你在构造器里写的

```cpp
G4int nofParticles = 1;
fParticleGun = new G4ParticleGun(nofParticles);
```

发射 `nofParticles` 个初级粒子。所以总共发射出去的初级粒子数就是：

> **N\_events × nofParticles\_per\_event**

——在你当前的配置下，`nofParticles_per_event=1`，如果你敲 `/run/beamOn 10`，那么就会发射 **10×1=10** 个 π⁺。

---

## 如何在代码里查询/打印

如果你想在代码里动态地“看”这个数字，可以在 `RunAction` 或 `EventAction` 里访问 `PrimaryGeneratorAction` 并询问 `G4ParticleGun`：

```cpp
// RunAction.hh 中加入：
#include "G4UserRunAction.hh"
class PrimaryGeneratorAction;  // 前向声明

class RunAction : public G4UserRunAction {
public:
  // ...  保持你原有的构造、析构
  void EndOfRunAction(const G4Run* run) override;
private:
  PrimaryGeneratorAction* fGenAction = nullptr;
};

// RunAction.cc
#include "RunAction.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "PrimaryGeneratorAction.hh"

void RunAction::BeginOfRunAction(const G4Run* /*run*/) {
  // 取出 PrimaryGeneratorAction 的指针，方便后面用
  fGenAction = static_cast<PrimaryGeneratorAction*>(
                 G4RunManager::GetRunManager()
                   ->GetUserPrimaryGeneratorAction());
}

void RunAction::EndOfRunAction(const G4Run* run) {
  G4int nEvents = run->GetNumberOfEvent();
  G4cout << "[RunAction] Processed events: " << nEvents << G4endl;

  if (fGenAction) {
    // 每个事件的初级粒子数
    G4int primPerEvt = fGenAction->GetParticleGun()->GetNumberOfParticles();
    G4cout << "[RunAction] Primaries per event: " << primPerEvt << G4endl;
    G4cout << "[RunAction] Total primaries: "
           << primPerEvt * nEvents << G4endl;
  }
}
```

只要在 `EndOfRunAction()` 里打印，运行结束时就会给你：

```
[RunAction] Processed events: 10
[RunAction] Primaries per event: 1
[RunAction] Total primaries: 10
```

---

## 如果想逐事件查看

你也可以在 `EventAction::EndOfEventAction()` 里打印：

```cpp
void EventAction::EndOfEventAction(const G4Event* event) {
  G4int npv = event->GetNumberOfPrimaryVertex();
  G4int nprim = 0;
  for (G4int i=0; i<npv; ++i) {
    nprim += event->GetPrimaryVertex(i)->GetNumberOfParticle();
  }
  G4cout << "[Event " << event->GetEventID()
         << "] Primaries this event: " << nprim << G4endl;
}
```

`GetNumberOfPrimaryVertex()` 给出顶点数（通常就是 1），而每个顶点的 `GetNumberOfParticle()` 才是真实发射的粒子数。这样，你可以逐事件知道到底发了几个初级粒子。

---

### 总结

* **宏里 `/run/beamOn N` → N 个事件，每事件发射 `G4ParticleGun` 里设置的 `nofParticles` 个粒子**；
* **代码中可通过 `G4ParticleGun::GetNumberOfParticles()` 和 `G4Run::GetNumberOfEvent()` 计算总数**；
* 或使用 `G4Event` 的接口逐事件查询实际的 primary 顶点/粒子数。

