#ifndef QUANTUMGATES_H

#define QUANTUMGATES_H

// Quantum NOT Gate:-
// Applies a NOT function to the probabilistic coefficients of the Qubit.
// This can also be assumed as rotation about X-axiz of the Bloch sphere
// For single qubit
quBit X(quBit x);
// For register of qubits
quReg* X_reg(quReg *qr, int idx);

// Rotation about Y-axis of the Bloch sphere
// For single qubit
quBit Y(quBit x);

// For register of qubits
quReg* Y_reg(quReg *qr, int idx);

// Rotation about Z-axis of the Bloch sphere
// For single qubit
quBit Z(quBit x);

// For register of qubits
quReg* Z_reg(quReg *qr, int idx);

// Quantum Hadamard Gate:-
// Applies hadamard Function to the Qubit passed into it.
// Puts the Qubit into superposition of |0> state and |1> state.
//Probability of getting both states after measuring is 50-50.
// For single qubit
quBit H(quBit x);

// For register of qubits
quReg* H_reg(quReg *qr, int idx);

//Quantum Phase Gate:-
// Changes the phase of the qubit
// For single qubit
quBit S(quBit x);

// For register of qubits
quReg* S_reg(quReg *qr, int idx);

// Quantum Rotation Gate:-
// Rotates the Coefficient of Quantum state about Complex Axis.
// angle should be in PI/k format, where k = 1, 2, 3, ...
// For single qubit
quBit R(double angle, quBit x);

// For register of qubits
quReg* R_reg(double angle, quReg *qr, int idx);

// Quantum CNOT Gate (Controlled NOT):-
// if (x1 == |1>) {x2 = X(x2)}
// else {x2 = x2}
// "Generally used for entanglement of the Qubits x1 and x2".
// For two different qubits
void CNOT(quBit *x1, quBit *x2);

// For register of qubits
quReg* CNOT_reg(quReg *qr, int idx1, int idx2);

// Quantum Swap Gate
// Swaps the two qubits
// For two different qubits
void QSwap(quBit *x1, quBit *x2);

// For register of qubits
quReg* QSwap_reg(quReg *qr, int idx1, int idx2);


// Short-hand for applying gates to registers
quReg* applyGates_reg(const char* gate_string, quReg *qr);



// Also some helper functions...
int get_prev_state(quReg *qr, int idx) {
	int prev_state = 0;

	for(int i = qr->size-1; i >= 0; i--) {
		if((__round(qr->qb[i].ZCoeff.real) == 1) ^ (__round(qr->qb[i].ZCoeff.imag) == 1)) {
			prev_state |= 0;
		}
		else if((__round(qr->qb[i].OCoeff.real) == 1) ^ (__round(qr->qb[i].OCoeff.imag) == 1)) {
			prev_state |= 1;
		}

		if(i > 0)
			prev_state <<= 1;
	}



	return prev_state;
}

int bit_parity(int i) {
	for(int j = 16; j > 0; j /= 2)
		i = (i >> j) ^ i;
	return (i % 2);
}

int product(int a, int b) {
	int i = a & b;
	for(int j = 16; j > 0; j /= 2)
		i = (i >> j) ^ i;
	return (i % 2);
}

Complex *mat;

void Init_matrix(quReg *qr) {
	mat = (newQuReg(qr->size)->matrix);
}

void updateMatrix(quReg *qr) {
	mat = qr->matrix;
}

// Function Definitions
//
//
//
//
//

quBit X(quBit x) {
	quBit qb = newQubit(0);

	qb.ZCoeff.real = x.OCoeff.real;
	qb.ZCoeff.imag = x.OCoeff.imag;
	
	qb.OCoeff.real = x.ZCoeff.real;
	qb.OCoeff.imag = x.ZCoeff.imag;

	return qb;
}

quReg* X_reg(quReg *qr, int idx) {
	int prev_state = 0, next_state = 0;

	prev_state = get_prev_state(qr, idx);

	next_state = prev_state ^ (1 << idx);

	qr->qb[idx] = X(qr->qb[idx]);

	// qr = updateMatrix(qr, idx);

	double temp = qr->matrix[prev_state].real;
	qr->matrix[prev_state].real = qr->matrix[next_state].real;
	qr->matrix[next_state].real = temp;

	return qr;
}

quBit Z(quBit x) {
	quBit qb = newQubit(0);

	qb.ZCoeff = x.ZCoeff;

	qb.OCoeff.real = x.OCoeff.real;
	qb.OCoeff.imag = -1 * x.OCoeff.imag;

	return qb;
}

quReg* Z_reg(quReg *qr, int idx) {
	int prev_state = 0, next_state = 0;

	prev_state = get_prev_state(qr, idx);

	next_state = prev_state ^ (1 << idx);

	double mod_p = sqrt(pow(qr->matrix[prev_state].real, 2) + pow(qr->matrix[prev_state].imag, 2));
	double mod_n = sqrt(pow(qr->matrix[next_state].real, 2) + pow(qr->matrix[next_state].imag, 2));;

	qr->qb[idx] = Z(qr->qb[idx]);

	if(fabs(__round(qr->qb[idx].OCoeff.real)) > 0 || fabs(__round(qr->qb[idx].OCoeff.imag)) > 0) {
		if(mod_n > 0) {
			qr->matrix[next_state].real *= -1;
			qr->matrix[next_state].imag *= -1;
		}
		else if(mod_p > 0) {
			qr->matrix[prev_state].real *= -1;
			qr->matrix[prev_state].imag *= -1;
		}
	}

	return qr;
}

quBit Y(quBit x) {
	quBit qb = newQubit(0);

	qb.ZCoeff.real = x.OCoeff.imag;
	qb.ZCoeff.imag = -1 * x.OCoeff.real;

	qb.OCoeff.real = -1 * x.ZCoeff.imag;
	qb.OCoeff.imag = x.ZCoeff.real;

	return qb;
}

quReg* Y_reg(quReg *qr, int idx) {
	int prev_state = 0, next_state = 0;
	int prev_bit = 0, next_bit = 0;
	int neg_z, neg_o;

	prev_state = get_prev_state(qr, idx);
	next_state = prev_state ^ (1 << idx);

	prev_bit = (prev_state & (1 << idx)) >> idx;
	next_bit = (next_state & (1 << idx)) >> idx;

	qr->qb[idx] = Y(qr->qb[idx]);

	if(prev_bit == 0 && next_bit == 1) {
		neg_z = -1;
		neg_o = 1;
	}
	else if(prev_bit == 1 && next_bit == 0) {
		neg_z = 1;
		neg_o = -1;
	}

	double t = qr->matrix[prev_state].real;
	qr->matrix[prev_state].real = neg_z * qr->matrix[next_state].imag;
	qr->matrix[next_state].imag = neg_o * t;

	t = qr->matrix[prev_state].imag;
	qr->matrix[prev_state].imag = neg_o * qr->matrix[next_state].real;
	qr->matrix[next_state].real = neg_z * t;

	return qr;
}

quBit H(quBit x) {
	quBit qb = newQubit(0);

	qb.ZCoeff.real = (x.ZCoeff.real + x.OCoeff.real) / sqrt(2);
	qb.OCoeff.real = (x.ZCoeff.real - x.OCoeff.real) / sqrt(2);

	qb.ZCoeff.imag = (x.ZCoeff.imag + x.OCoeff.imag) / sqrt(2);
	qb.ZCoeff.imag = (x.ZCoeff.imag - x.OCoeff.imag) / sqrt(2);

	return qb;
}

quReg* H_reg(quReg *qr, int idx) {
	Complex *temp = (newQuReg(qr->size)->matrix);
	Complex *sample = mat;

	temp[0].real = 0;

	for(int i = 0; i < pow(2, (idx+1)); i++) {
		for(int j = 0; j < pow(2, (idx+1)); j++) {
			int prod = product(i, j);
			double div = pow(2, (double)(idx+1)/2);
			double H_ij = pow(-1, prod) / div;
			temp[i].real = temp[i].real + H_ij * sample[j].real;
			temp[i].imag = temp[i].imag +  H_ij * sample[j].imag;
		}
	}

	for(int i = 0; i < pow(2, (idx+1)); i++) {
		qr->matrix[i] = temp[i];
	}
	return qr;
}

quBit S(quBit x) {
	quBit qb = newQubit(0);

	qb.ZCoeff = x.ZCoeff;

	qb.OCoeff.real = -1 * x.OCoeff.imag;
	qb.OCoeff.imag = x.OCoeff.real;

	return qb;
}

quReg* S_reg(quReg *qr, int idx) {
	int prev_state, next_state;

	prev_state = get_prev_state(qr, idx);

	// next_state;

	qr->qb[idx] = S(qr->qb[idx]);

	return qr;
}

quBit R(double angle, quBit x) {
	quBit qb = newQubit(0);

	qb.ZCoeff = x.ZCoeff;

	qb.OCoeff.real = x.OCoeff.real * cos(angle) - x.OCoeff.imag * sin(angle);
	qb.OCoeff.imag = x.OCoeff.real * sin(angle) + x.OCoeff.imag * cos(angle);

	return qb;
}

quReg* R_reg(double angle, quReg *qr, int idx) {
	int prev_state, next_state;

	prev_state = get_prev_state(qr, idx);

	// next_state;

	qr->qb[idx] = R(angle, qr->qb[idx]);

	return qr;
}

void CNOT(quBit *x1, quBit *x2) {
	quBit qb = newQubit(0);

	if(x1->OCoeff.real == 1 && x1->OCoeff.imag == 0)
		qb = X(*x2);

	*x2 = qb;
}

quReg* CNOT_reg(quReg *qr, int idx1, int idx2) {
	int prev_state1, next_state1, prev_state2, next_state2;

	prev_state1 = get_prev_state(qr, idx1);
	prev_state2 = get_prev_state(qr, idx2);

	// next_state1;
	// next_state2;

	CNOT(&qr->qb[idx1], &qr->qb[idx2]);

	return qr;
}

void QSwap(quBit *x1, quBit *x2) {
	quBit temp = *x1;
	*x1 = *x2;
	*x2 = temp;
}

quReg* QSwap_reg(quReg *qr, int idx1, int idx2) {
	int prev_state1, next_state1, prev_state2, next_state2;

	prev_state1 = get_prev_state(qr, idx1);
	prev_state2 = get_prev_state(qr, idx2);

	// next_state1;
	// next_state2;

	QSwap(&qr->qb[idx1], &qr->qb[idx2]);

	return qr;
}


quReg* applyGates_reg(const char* gate_string, quReg *qr) {
	int start_col = 0;
	int start_func = 0;
	int text_start = 0;

	int gate_num = 0;

	char msg[25] = "\0";
	int msg_idx = 0;

	Init_matrix(qr);

	for(int i = 0; gate_string[i] != '\0'; i++) {
		if(gate_string[i] == '(') {
			i++;
			if(gate_string[i] == 'T') {
				i++;
				while(gate_string[i] != ')') {
					if((gate_string[i] == ' ' || gate_string[i] == ':') && text_start == 0) {
					}
					else {
						msg[msg_idx] = gate_string[i];
						msg_idx++;
						text_start = 1;
					}
					i++;
				}
				msg[msg_idx] = '\0';
			}
			else if(gate_string[i] == 'P') {
				i++;
				if(gate_string[i] == 'a') {
					printf("%s", msg);
					Qprint("%a\n", qr);
				}
				else if(gate_string[i] == ')' || (gate_string[i] == 'n' && gate_string[++i] == 'z')) {
					printf("%s", msg);
					Qprint("%r\n", qr);
				}
			}
		}
		else if(gate_string[i] == '{')
			start_func = 1;
		else if(gate_string[i] == '}')
			start_func = 0;

		else if(start_func) {
			if(start_col == 0 && gate_string[i] == '[')
				start_col = 1;
			else if(gate_string[i] == ']') {
				start_col = 0;
				gate_num = 0;
			}

			if(start_col) {
				if(gate_string[i] == ',' || gate_string[i] == ' ') {

				}
				else if(gate_string[i] == 'X') {
					qr = X_reg(qr, gate_num);
					updateMatrix(qr);
					gate_num = (gate_num+1) % qr->size;
				}
				else if(gate_string[i] == 'Y') {
					qr = Y_reg(qr, gate_num);
					updateMatrix(qr);
					gate_num = (gate_num+1) % qr->size;
				}
				else if(gate_string[i] == 'Z') {
					qr = Z_reg(qr, gate_num);
					updateMatrix(qr);
					gate_num = (gate_num+1) % qr->size;
				}
				else if(gate_string[i] == 'H') {
					qr = H_reg(qr, gate_num);
					gate_num = (gate_num+1) % qr->size;
				}
				else if(gate_string[i] == '1') {
					gate_num = (gate_num+1) % qr->size;
				}
			}
		}

		else {
			
		}
	}

	return qr;
}

#endif
