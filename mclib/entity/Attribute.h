#ifndef MCLIB_ENTITY_ATTRIBUTE_H_
#define MCLIB_ENTITY_ATTRIBUTE_H_

#include <common/Types.h>
#include <common/UUID.h>




enum class ModifierOperation
{
	// Add/subtract amount. Applied first.
	Add,
	// Add/subtract percent of current value. Applied second.
	AddPercent,
	// Multiply by percent. Applied last.
	MultiplyPercent
};

class Modifier
{
private:
	CUUID m_UUID;
	double m_Amount;
	ModifierOperation m_Operation;

public:
	Modifier(CUUID uuid, double amount, ModifierOperation operation)
		: m_UUID(uuid),
		m_Amount(amount),
		m_Operation(operation)
	{}

	inline const CUUID& GetUUID() const noexcept { return m_UUID; }
	inline double GetAmount() const noexcept { return m_Amount; }
	inline ModifierOperation GetOperation() const noexcept { return m_Operation; }
};

class Attribute
{
public:
	using Modifiers = std::vector<Modifier>;

private:
	std::wstring m_Key;
	double m_Amount;
	Modifiers m_Modifiers;

public:
	Attribute(const std::wstring& key, double amount)
		: m_Key(key),
		m_Amount(amount)
	{

	}

	inline const std::wstring& GetKey() const noexcept { return m_Key; }
	inline double GetBaseAmount() const noexcept { return m_Amount; }
	inline const Modifiers& GetModifiers() const noexcept { return m_Modifiers; }

	// Returns the amount after being modified.
	double GetAmount() const
	{
		double amount = m_Amount;

		for (const auto& modifier : m_Modifiers)
		{
			if (modifier.GetOperation() == ModifierOperation::Add)
			{
				amount += modifier.GetAmount();
			}
		}

		double addedAmount = amount;
		for (const auto& modifier : m_Modifiers)
		{
			if (modifier.GetOperation() == ModifierOperation::AddPercent)
			{
				amount += addedAmount * modifier.GetAmount();
			}
		}

		for (const auto& modifier : m_Modifiers)
		{
			if (modifier.GetOperation() == ModifierOperation::MultiplyPercent)
			{
				amount *= (1 + modifier.GetAmount());
			}
		}

		return amount;
	}

	void AddModifier(const Modifier& modifier)
	{
		m_Modifiers.push_back(modifier);
	}
};

#endif
