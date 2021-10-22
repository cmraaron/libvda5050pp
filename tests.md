# Test Coverage Overview

## Notation

- state."field" denotes elements in the vda5050 state
- "[]" denotes a list/-access
- "[*]" denotes "for all"
- "[+]" denotes "exists"
- "[>=n]" denotes "at least n"
- "[name]" denotes the elements that correspond to an object "name"


## 1 Instant Actions

### 1.1 Cancel 

- [X] 1.1.1 [Manual] No Order 
  Pre:
  - state.nodeStates = []
  - state.edgeStates = []
  - receive cancelOrder

  Post:
  - "noOrderToCancel" in state.errors

- [X] 1.1.2 [Manual] Operates normally after canceling  
  Pre:  
  - Load Order
  - receive cancelOrder

  Post:  
  - state.nodeStates = []
  - state.edgeStates = []
  - can execute next order

- [X] 1.1.3 [Manual] Status when canceling  
  Pre:
  - Load Order
  - Receive cancelOrder

  Post:  
  - state.actionStates[cancelAction].status = running
  - status.driving = false
  - state.actionStates[*].status = finished|failed
  - state.actionStates[cancelAction].status = finished

### 1.2 Pause 

- [X] 1.2.1 [Manual] Not pausable (AGVs decision)  
  Pre:
  - Load Order
  - state.actionStates[*].status =/= paused
  - state.paused = false
  - receive startPause
  - AGV.pauseResumeCallback ~> fail (AGV can not pause)

  Post:
  - state.actionStates[*].status =/= paused
  - state.actionStates[startPauseAction].status = failed
  - state.paused = false


- [X] [Manual] 1.2.2 No Order
  Pre:
  - state.nodeStates = []
  - state.edgeStates = []
  - receive startPause
  - receive Order

  Post:
  - state.paused = true
  - state.actionStates[*].status = paused
  - state.driving = false

### 1.3 Resume 

- [X] 1.3.1 [Manual] Not resumable (AGVs decision)  
  Pre:
  - Load Order
  - state.actionStates[*].status =/= running
  - state.paused = true
  - receive stopPause
  - AGV.pauseResumeCallback ~> fail (AGV can not resume)

  Post:
  - state.actionStates[*].status =/= running
  - state.actionStates[stopPauseAction].status = failed
  - state.paused = true


- [X] 1.3.2 [Manual] No Order  
  Pre:
  - state.nodeStates = []
  - state.edgeStates = []
  - receive stopPause
  - receive Order

  Post:
  - state.paused = false
  - state.actionStates[*].status =/= paused

### 1.4 StateRequest 

- [X] 1.4.1 [Manual] Send State  
  Pre:
  - received sendState

  Post:
  - state sent

### 1.5 LogReport 

- [X] 1.5.1 [Manual] Forward to AGV
  Pre:
  - received logReport

  Post:
  - AGV logged

### 1.6 Preemption

- [X] 1.6.1 [Automatic] Receive Order after InstantAction  
  Pre:
  - received InstantAction
  - state.actionStates[+] == running
  - receive Order

  Post:
  - state.actionStates[+] == running
  - state.driving = false

- [X] 1.6.2 [Automatic] Receive HARD InstantAction while running NONE Action  
  Pre:
  - receive order (n1)
  - n1.actions[*].blockingType == NONE
  - state.driving = true
  - receive HARD InstantAction

  Post:
  - state.actionStates[+] == running
  - state.actionStates[on n1] == finished | failed
  - state.driving = false

- [X] 1.6.3 [Automatic] Receive SOFT InstantAction while running NONE Action  
  Pre:
  - receive order (n1)
  - n1.actions[*].blockingType == NONE
  - state.driving = true
  - receive SOFT InstantAction

  Post:
  - state.actionStates[+] == running
  - state.actionStates[on n1] == running
  - state.driving = false

- [X] 1.6.4 [Automatic] Receive NONE InstantAction while running NONE Action  
  Pre:
  - receive order (n1)
  - n1.actions[*].blockingType == NONE
  - state.driving = true
  - receive NONE InstantAction

  Post:
  - state.actionStates[+] == running
  - state.actionStates[on n1] == running
  - state.driving = true

- [X] 1.6.5 [Automatic] Receive HARD InstantAction while running SOFT Action  
  Pre:
  - receive order (n1)
  - n1.actions[*].blockingType == SOFT
  - state.driving = false
  - receive HARD InstantAction

  Post:
  - state.actionStates[+] == running
  - state.actionStates[on n1] == finished | failed
  - state.driving = false

- [X] 1.6.6 [Automatic] Receive SOFT InstantAction while running SOFT Action  
  Pre:
  - receive order (n1)
  - n1.actions[*].blockingType == SOFT
  - state.driving = false
  - receive SOFT InstantAction

  Post:
  - state.actionStates[+] == running
  - state.actionStates[on n1] == running
  - state.driving = false

- [X] 1.6.7 [Automatic] Receive NONE InstantAction while running SOFT Action  
  Pre:
  - receive order (n1)
  - n1.actions[*].blockingType == SOFT
  - state.driving = false
  - receive NONE InstantAction

  Post:
  - state.actionStates[+] == running
  - state.actionStates[on n1] == running
  - state.driving = false

- [X] 1.6.8 [Automatic] Receive HARD InstantAction while running HARD Action  
  Pre:
  - receive order (n1)
  - n1.actions[*].blockingType == HARD
  - state.driving = false
  - receive HARD InstantAction

  Post:
  - state.actionStates[+] == running
  - state.actionStates[on n1] == finished | failed
  - state.driving = false

- [X] 1.6.9 [Automatic] Receive SOFT InstantAction while running HARD Action  
  Pre:
  - receive order (n1)
  - n1.actions[*].blockingType == HARD
  - state.driving = false
  - receive SOFT InstantAction

  Post:
  - state.actionStates[+] == running
  - state.actionStates[on n1] == finished | failed
  - state.driving = false

- [X] 1.6.10 [Automatic] Receive NONE InstantAction while running HARD Action  
  Pre:
  - receive order (n1)
  - n1.actions[*].blockingType == HARD
  - state.driving = false
  - receive NONE InstantAction

  Post:
  - state.actionStates[+] == running
  - state.actionStates[on n1] == finished | failed
  - state.driving = true

## 2 State Update Triggers

- [X] 2.1 [Manual] Receive Order or Order update
- [X] 2.2 [Manual] Changed Load
- [X] 2.3 [Manual] Error
- [X] 2.4 [Manual] Node reached
- [X] 2.5 [Manual] OperatingMode changed
- [X] 2.6 [Manual] Driving changed
- [X] 2.7 [Manual] {node,edge,action}State changed

## 3 Action Triggers

- [X] 3.1 [Automatic] Start when Node is reached  
  Pre:
  - Load Order
  - n1.actions =/= []
  - enter node n1

  Post:
  - n1.actions[+].status =/= waiting (at least one not waiting)
  
- [X] 3.2 [Automatic] Block driving when action is HARD/SOFT blocking  
  Pre:
  - Load Order
  - n1.actions[+].blockingType =/= NONE
  - enter node n1

  Post:
  - state.driving = false
  - n1.actions[+].status =/= waiting

- [X] 3.3 [Automatic] Do not run HARD blocking actions in parallel  
  Pre:
  - Load Order
  - enter node n1
  - n1.actions[2>=].blockingType = HARD

  Post:
  - only one HARD blocking action running

- [X] 3.4 [Automatic] Cancel Edge-actions, when Edge is left  
  Pre:
  - Load Order
  - e1.actions[*] =/= []
  - e1.actions[*].blockingType = NONE
  - e1.actions[*].status = running
  - enter edge e1
  - enter n2 (after e1)

  Post:
  - e1.actions[*].status = finished|failed

- [X] 3.5 [Automatic] Action order (as in the list)  
  Pre:
  - Load Order
  - n1.actions = {a1 ... a4}
  - n1.actions[*].blockingType = HARD
  - enter n1

  Post:
  - status.actionState[a_i].status = running
  - status.actionState[a_i+1].status = waiting
  - ...
  - status.actionState[*].status = finished | failed
  

## 4 Header 

- [X] 4.1 [Manual] Increment IDs
- [X] 4.2 [Manual] timestamp ISO8601 UTC
- [X] 4.3 [Automatic] protocol version matches VDA5050 version
- [X] 4.4 [Automatic] only respond to correct manufacturer/SN combination
- [ ] 4.5 log headerID skips

## 5 Order 

- [X] 5.1 [Manual] Only navigate inside of the base  
  Pre:
  - Send Order with Base n1..n3 and Horz n4..n5
  
  Post:
  - state.nodeStates = [n1..n5]
  - state.driving = true
  - ...
  - state.nodeStates = [n4..n5]
  - state.driving = false

- [X] 5.2 [Manual] Error when base is changed  
  Pre:
  - Receive Order with Base n1..n3
  - Receive Order with Base n3..n1

  Post:
  - state.nodeStates = [n1..n3]
  - state.errors contains "orderError"

- [ ] 5.3 [Skip] Validate order of Nodes and Edges in the list (Order is maintained and checked via seqIds)
  Pre:
  - state.nodeStates = []
  - Receive Order with Nodes n3..n1

  Post:
  - state.nodeStates = []
  - state.errors contains "orderError"

- [X] 5.4 Clear horizon on update  
  Pre:
  - Receive Order with Base n1..n3 and Horz n4..n5
  - Receive Order with Base n1..n3 and Horz n7..n8

  Post:
  - state.nodeStates = [n1..n3, n7..n8]


- [ ] 5.6 [Skip] check reachability of the first Node (AGV can fail DriveToNodeTask, but not decide while validating the order)
  Pre:
  - AGV is not in reach of n1
  - receive Order beginning with n1

  Post:
  - state.errors =/= []

- [X] 5.7 [Automatic] check sequence number consistency  
  Pre:
  - Load Order containing sequence ids [1,2,4,5]

  Post:
  - state.errors =/= []


## 6 Generic Validation

- [ ] 6.1 set "validationError" on invalid order syntax  
  Pre:
  - receive message with invalid syntax

  Post:
  - state.errors contains "validationError"

- [X] 6.2 [Automatic] set "orderUpdateError" on invalid ids
  Pre:
  - state.orderId = id
  - receive Order with orderId = id2 and orderUpdateId = 1
  
  Post:
  - state.errors contains "orderUpdateError"

- [X] 6.3 [Manual] ignore duplicate orderID orderUpdateID combination  
  Pre:
  - state.orderId = id
  - state.orderUpdateId = 1
  - state.nodeStates = [n1..n3]
  - state.errors = []
  - receive Order with orderId = id and orderUpdateId = 1
    and nodes [n4..n7]

  Post:
  - state.orderId = id
  - state.orderUpdateId = 1
  - state.errors = []
  - state.nodeStates = [n1..n3]

## (7) Connection Topic

- [ ] 7.1 Send "last-will"
- [X] 7.2 [manual] send online/offline message
