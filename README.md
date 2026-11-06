## 📐 Networking & Systems Architecture

The project follows a **listen-server architecture** with server-authoritative gameplay. Clients handle input and local interaction detection, while the server validates requests, controls gameplay state changes, and replicates the results to connected players.

### 1. Server-Authoritative Interaction Validation

The interaction system separates client-side target detection from server-side gameplay authorization.

```text
Client presses E
       ↓
Local Camera Trace (600 units)
       ↓
Interaction Target Detected
       ↓
Client sends Server RPC
       ↓
Server validates target and distance
       ↓
Authorized Interaction
       ↓
Server modifies gameplay state
       ↓
Replication to connected clients
```

**Interaction validation:**

- Client interaction trace range: **600 units**
- Server-authorized interaction range: **300 units**
- The server independently checks the interaction target and player distance.
- Invalid or out-of-range requests are rejected.
- Clients cannot directly modify authoritative gameplay state.

This demonstrates server-side gameplay validation rather than trusting client requests.

### 2. Replicated Gameplay State

The project implements replicated gameplay actors using Unreal Engine's property replication system.

**Replicated Switch**

- Uses `ReplicatedUsing` to track the switch's active state.
- Implements `OnRep_IsActive()` for client-side visual synchronization.
- The server controls state changes.
- The switch visually tilts approximately 35 degrees when activated.

**Replicated Pickup**

- Clients request pickup collection through a Server RPC.
- The server validates the pickup interaction.
- The server increments the collecting player's `PickupCount`.
- The server destroys the pickup actor.
- Actor destruction and updated player state replicate across the network.

### 3. Per-Player State Synchronization

A custom `AMultiplayerPlayerState` manages individual player data.

- Implements a replicated `PickupCount` variable using `ReplicatedUsing`.
- Uses `GetLifetimeReplicatedProps()` for property replication.
- Implements `OnRep_PickupCount()` to respond to replicated count changes.
- Maintains independent pickup counts for each connected player.

Example:

```text
HOST POV             CLIENT POV
PICKUPS: 1           PICKUPS: 2
```

Each player's pickup count is maintained independently rather than using a shared global counter.

### 4. Event-Driven Multiplayer UI

The HUD communicates with gameplay systems through dynamic multicast delegates.

- Implements `OnPickupCountChanged` for pickup-count updates.
- Updates the local HUD when the server changes player state or when replicated state arrives.
- Avoids continuous Tick-based polling for pickup-count updates.
- Displays HOST POV / CLIENT POV identification and individual pickup counts.
- Includes loading indicators and user-facing session and connection failure messages.

### 5. Multiplayer Session & Travel Architecture

The session system is built around `UGameInstanceSubsystem` and `OnlineSubsystemNull`.

**Session Management**
- LAN session creation and discovery.
- Automatic six-character room-code generation.
- Advertised `ROOM_CODE` session settings.
- Case-insensitive room-code matching.
- Session joining, destruction, and recreation.
- Asynchronous session delegate management.
- Connection and session failure handling.

**Synchronized Travel**
- Server-side player-readiness checks.
- Uses `HasClientLoadedCurrentWorld()` as part of the readiness workflow.
- Configures seamless travel through `bUseSeamlessTravel`.
- Uses server-controlled `ServerTravel` to transition connected players into `NetTestMap`.
- Includes a stabilization period before initiating travel.

### 6. Core C++ Architecture

| Class | Responsibility |
|---|---|
| `UMultiplayerSessionSubsystem` | LAN session creation, discovery, joining, and destruction |
| `UMultiplayerGameInstance` | UI-to-session integration |
| `AMultiplayerGameMode` | Player readiness and server-controlled travel |
| `AMultiplayerCharacter` | Enhanced Input, interaction traces, and Server RPCs |
| `AMultiplayerPlayerState` | Replicated per-player pickup count |
| `AReplicatedSwitch` | RepNotify-based switch state synchronization |
| `AReplicatedPickup` | Server-authoritative pickup collection and destruction |

### 7. Development & Testing

- Developed using **Unreal Engine 5.8.2 and C++**.
- Tested using separate standalone host and client instances.
- Verified LAN session creation, room-code matchmaking, and client joining.
- Tested replicated gameplay interactions between connected players.
- Verified independent per-player pickup counts.
- Tested server-side interaction distance validation.
- Validated synchronized multiplayer travel.
- Compiled successfully using the **Development Editor x64** configuration.
