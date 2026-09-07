# UE5 Multiplayer Networking Demo

A focused **Unreal Engine 5.8.2 multiplayer prototype** built primarily in **C++** to demonstrate client-server networking, server authority, RPCs, replication, session management, and PlayerState synchronization.

The project intentionally keeps gameplay simple and focuses on **multiplayer architecture and reliability**.

## Key Features

- Room-code-based LAN session creation, discovery, and joining
- Listen-server / client multiplayer architecture
- Seamless synchronized travel with player-readiness checks
- Server-authoritative interactions with distance validation
- Replicated switch using **RepNotify**
- Server-authoritative replicated pickup system
- Per-player replicated pickup count using **PlayerState**
- Multiplayer HUD with **HOST POV / CLIENT POV** identification
- Session, join, and connection failure handling

## Networking Architecture

```text
Client Input
    ↓
Local Interaction Trace
    ↓
Owned Character
    ↓
Server RPC
    ↓
Server Validation
    ↓
Authoritative State Change
    ↓
Replication
    ↓
Client Presentation
````

Clients request actions, but the **server remains authoritative** and independently validates interactions before modifying gameplay state.

## Replicated Gameplay

### Replicated Switch

```text
Press E
→ Server RPC
→ Server validates distance
→ bIsActive changes
→ RepNotify
→ Switch synchronizes on all clients
```

### Replicated Pickup

```text
Press E
→ Server RPC
→ Server validates pickup
→ PlayerState PickupCount++
→ Server destroys pickup
→ Count and actor removal replicate
```

Each player maintains an independent replicated pickup count.

Example:

```text
HOST POV
PICKUPS: 1

CLIENT POV
PICKUPS: 2
```

## Core C++ Classes

```text
MultiplayerSessionSubsystem
→ Create / Find / Join / Destroy sessions

MultiplayerGameMode
→ Player readiness + synchronized ServerTravel

MultiplayerGameInstance
→ UI-to-session bridge

MultiplayerCharacter
→ Input, interaction traces, Server RPCs

MultiplayerPlayerState
→ Replicated per-player PickupCount

ReplicatedSwitch
→ RepNotify-based replicated state

ReplicatedPickup
→ Server-authoritative replicated destruction
```

## Tech Stack

**Unreal Engine 5.8.2 · C++ · Blueprints · UMG · Enhanced Input · OnlineSubsystemNull · RPCs · RepNotify · PlayerState · Seamless Travel**

## Controls

| Input | Action   |
| ----- | -------- |
| WASD  | Move     |
| Mouse | Look     |
| Space | Jump     |
| E     | Interact |

## Demo

Demo video coming soon.

## Author

**Asif Tanjim**

Gameplay / Multiplayer Systems Developer
