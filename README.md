# UE5 Multiplayer Networking Demo

A focused **Unreal Engine 5.8.2 multiplayer networking demo** built primarily in **C++**, demonstrating client-server architecture, server authority, RPCs, replication, session management, synchronized travel, and PlayerState synchronization.

The project intentionally keeps gameplay simple so the focus remains on **multiplayer architecture, networking concepts, validation, synchronization, and reliability**.

## Key Features

- Room-code-based LAN session creation, discovery, and joining
- Listen-server / client multiplayer architecture
- Session lifecycle management with failure handling
- Seamless synchronized travel with player-readiness checks
- Server-authoritative gameplay interactions
- Server-side distance validation
- Client → Server RPC communication
- Replicated switch using **RepNotify**
- Server-authoritative replicated pickup system
- Per-player replicated pickup count using **PlayerState**
- Event-driven multiplayer HUD updates
- HOST POV / CLIENT POV identification
- Connection, join, and session failure handling

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
```

Clients request gameplay actions, but the **server remains authoritative**.

Client requests are independently validated by the server before any gameplay state is modified.

## Replicated Gameplay

### Replicated Switch

```text
Press E
    ↓
Server RPC
    ↓
Server validates interaction
    ↓
Server validates distance
    ↓
bIsActive changes
    ↓
RepNotify
    ↓
Switch state synchronizes across clients
```

The client can request an interaction from a longer trace distance, while the server enforces a shorter authoritative interaction range.

```text
Client trace range: 600 units
Server validation range: 300 units
```

This demonstrates that the client does not have final authority over gameplay interactions.

### Replicated Pickup

```text
Press E
    ↓
Server RPC
    ↓
Server validates pickup
    ↓
PlayerState PickupCount++
    ↓
Server destroys pickup
    ↓
State and actor removal replicate
```

The client never directly destroys the pickup.

Each player maintains an independent replicated pickup count through their own `PlayerState`.

Example:

```text
HOST POV
PICKUPS: 1

CLIENT POV
PICKUPS: 2
```

## Core C++ Architecture

```text
MultiplayerSessionSubsystem
→ Create / Find / Join / Destroy sessions

MultiplayerGameMode
→ Player readiness + synchronized ServerTravel

MultiplayerGameInstance
→ UI-to-session system bridge

MultiplayerCharacter
→ Input, interaction traces, Server RPCs

MultiplayerPlayerState
→ Replicated per-player PickupCount

ReplicatedSwitch
→ RepNotify-based replicated state

ReplicatedPickup
→ Server-authoritative pickup, collection, and destruction
```

## Session & Travel Flow

```text
Host
  ↓
Create LAN Session
  ↓
Generate Room Code
  ↓
Client enters Room Code
  ↓
Find LAN Sessions
  ↓
Match Room Code
  ↓
Join Session
  ↓
Client connects to Host
  ↓
Server detects required players
  ↓
Readiness checks
  ↓
Stabilization period
  ↓
Seamless ServerTravel
  ↓
NetTestMap
```

## What This Demonstrates

- Unreal Engine C++ multiplayer development
- Client/server responsibility separation
- Listen-server architecture
- Server authority
- Actor ownership
- Client → Server RPCs
- Server-side gameplay validation
- Replicated properties
- RepNotify
- Replicated actor destruction
- PlayerState replication
- Per-player networked state
- Event-driven UI updates
- Enhanced Input
- OnlineSubsystem session management
- Room-code matchmaking
- Session lifecycle management
- Seamless multiplayer travel
- Client readiness synchronization
- Multiplayer failure handling
- Network timing and synchronization debugging

## Tech Stack

**Unreal Engine 5.8.2 · C++ · Blueprints · UMG · Enhanced Input · OnlineSubsystemNull · RPCs · RepNotify · PlayerState · Seamless Travel**

## Controls

| Input | Action |
|-------|--------|
| WASD | Move |
| Mouse | Look |
| Space | Jump |
| E | Interact |

## Screenshots

### Multiplayer Lobby

_Add screenshot here._

### Replicated Gameplay

_Add screenshot here._

### Per-Player Networked State

_Add screenshot here._

## Demo

A short gameplay and networking demonstration will be added here.

**Demo Video:** Coming soon.

## Project Structure

```text
MultiplayerDemo/
├── Config/
├── Content/
├── Source/
│   └── MultiplayerDemo/
├── MultiplayerDemo.uproject
├── README.md
├── .gitignore
└── .gitattributes
```

Generated Unreal Engine folders such as `Binaries`, `Intermediate`, `Saved`, and `.vs` are excluded from the repository.

## Author

**Asif Tanjim**

Gameplay / Multiplayer Systems Developer

Focused on **Unreal Engine C++ gameplay programming and multiplayer systems**.
