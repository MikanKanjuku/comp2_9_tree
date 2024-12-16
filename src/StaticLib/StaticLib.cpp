#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#include "Windows.h"                    // Windows API の機能定義
#include <stdlib.h>

#include "../include/lib_func.h"


// 2分木の初期化
void initialize(tree* t)
{
	if (t == NULL) return;

	t->root = NULL;
}

static void release_recursive(node* n)
{
	if (n == NULL) return;

	if (n->right) {
		release_recursive(n->right);
		n->right = NULL;
	}

	if (n->left) {
		release_recursive(n->left);
		n->left = NULL;
	}

	free(n);
}

// 使用メモリの全解放
void finalize(tree* t)
{
	if (t == NULL) return;

	release_recursive(t->root);
	t->root = NULL;
}


static node* generate(int key, const char* value)
{
	node* p = (node*)malloc(sizeof(node));

	if (p == NULL) return NULL;

	p->key = key;
	int n = (int)strlen(value);
	memcpy(p->value, value, strlen(value)+1);

	p->left = p->right = NULL;

	return p;
}

// keyの値を見てノードを追加する
bool add(tree* t, int key, const char* value)
{
	if (t == NULL) return false;

	node* p = generate(key, value);
	if (p == NULL) return false;// メモリ確保できなかった。

	if (t->root == NULL) {
		t->root = p;
		return true;
	}

	// Todo: t->rootの下にkeyの値の大小でleftかrightを切り替えながらpを追加する処理を実装する
	node* current = t->root;
	while (true) {
		if (key < current->key) {
			if (current->left == NULL) {
				current->left = p;
				return true;
			}
			current = current->left;
		}
		else if (key > current->key) {
			if (current->right == NULL) {
				current->right = p;
				return true;
			}
			current = current->right;
		}
		else {
			// すでに同じキーが存在する場合は失敗
			free(p);
			return false;
		}
	}
	return true;
}

// keyの値を見てノードを検索して、値を取得する
const char* find(const tree* t, int key)
{
	if (t == NULL || t->root == NULL) return NULL;

	node* current = t->root;
	while (current != NULL) {
		if (key == current->key) {
			return current->value;
		}
		else if (key < current->key) {
			current = current->left;
		}
		else {
			current = current->right;
		}
	}
	return NULL; // 見つからなかった
}

// keyの小さな順にコールバック関数funcを呼び出す
void search(const tree* t, void (*func)(const node* p))
{
	if (t == NULL || func == NULL) return;

	node* stack[100]; // スタックのサイズは適宜調整してください
	int top = -1;     // スタックのインデックス
	node* current = t->root;

	// 中間順巡回 (非再帰版)
	while (current != NULL || top != -1)
	{
		// 左部分木をスタックに積む
		while (current != NULL)
		{
			stack[++top] = current; // 現在のノードをスタックに追加
			current = current->left; // 左部分木に移動
		}

		// 左端に到達したらスタックからノードを取得して処理
		current = stack[top--];
		func(current); // コールバック関数を呼び出し

		// 右部分木を探索する
		current = current->right;
	}
}