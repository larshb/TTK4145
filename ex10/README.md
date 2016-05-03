Exercise 10
===========

### Safety/Deadlock Assignment

This is a system with a deadlock:

	RESOURCE = (get->put->RESOURCE).
	P = (printer.get -> scanner.get -> copy -> scanner.put -> printer.put -> P).
	Q = (scanner.get -> printer.get -> copy -> printer.put -> scanner.put -> Q).
	||SYS = (p:P || q:Q || {p,q}::printer:RESOURCE || {p,q}::scanner:RESOURCE).

The LTSA detects it (using <kbd>Check | Safety</kbd>) with the following output:

	Trace to DEADLOCK:
		p.printer.get
		q.scanner.get


To detect a deadlock from the FSP model itself one can go through it step-by-step to manually search for a deadlock.

### Dining philosophers

FSP Model (with 3 philosophers):

	P1 = (p1get1 -> p1get2 -> p1put2 -> p1put1 -> P1).
	P2 = (p2get2 -> p2get3 -> p2put3 -> p2put2 -> P2).
	P3 = (p3get3 -> p3get1 -> p3put1 -> p3put3 -> P3).
	F1 = (p1get1 -> p1put1 -> F1 | p3get1 -> p3put1 -> F1).
	F2 = (p2get2 -> p2put2 -> F2 | p1get2 -> p1put2 -> F2).
	F3 = (p3get3 -> p3put3 -> F3 | p2get3 -> p2put3 -> F3).
	||SYSTEM = (P1 || P2 || P3 || F1 || F2 || F3).

The system goes into a deadlock when every philosopher (P) picks up one fork each.

### Communication

	T1 = (e1 -> e2 -> T1).
	T2 = (e1 -> e2 -> T2).

There will be two states in the transition diagram.

When the channels are buffered, we need to use a bounded buffer strategy to avoid deadlocks.
